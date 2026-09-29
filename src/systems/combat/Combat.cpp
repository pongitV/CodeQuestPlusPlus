#include "Combat.h"
#include "CombatUIImpl.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>
#include <thread>
#include <map>
#include <chrono>

#include "../../entities/classes/ClassBase.h"
#include "../inventory/InventoryCombat.h"
#include "../inventory/Item.h"
#include "../inventory/equipment/ShieldEquipment.h"
#include "../../entities/enemies/mahoraga/Mahoraga.h"
#include "../../entities/races/RaceBase.h"
#include "../progress/Bestiary.h"
#include "../progress/Diary.h"
#include "../progress/Progression.h"
#include "../progress/ProgressionFlags.h"
#include "Parry.h"
#include "../../core/utils/RandomGenerator.h"
#include "../../core/utils/InputControl.h"
#include "../../core/state/Debug.h"
#include "../../core/utils/DialogFunctions.h"
#include "./mechanics/DamageCalculator.h"
#include "./mechanics/EnemyMechanics.h"
#include "./mechanics/TurnManager.h"
#include "../../core/utils/Color.h"

#include "../../ui/screens/combat/ScreenCombat.h"

namespace {
    void registrarLog(const std::string& texto, Color /*cor*/ = Color::RESET) {
        if (texto.empty()) return;
        TelaCombate::adicionarMensagemFixa(texto);
    }
}

void Combat::resetAdvancedStatistics() {
    stats_attemptedParries = 0;
    stats_effectiveParries = 0;
    stats_perfectParries = 0;
    stats_highestDamageDealt = 0;
    stats_consumedItems = 0;
    stats_newDiscoveries.clear();
}

Combat::Combat(Character* combatPlayer,
               std::vector<std::unique_ptr<Character>>&& combatEnemies,
               std::unique_ptr<ICombatUI> visualInterface)
    : currentPlayer(combatPlayer), enemyList(std::move(combatEnemies)), goldEarned(0), xpEarned(0), totalDamageDealt(0), totalDamageTaken(0), currentTurnCounter(1),
      ui(visualInterface ? std::move(visualInterface) : std::make_unique<CombatUIImpl>())
{
    int difficultyLevel = static_cast<int>(currentPlayer->getDifficulty());
    double enemyDifficultyMultiplier = 1.0;

    if (difficultyLevel == 2) {
        enemyDifficultyMultiplier = 1.5;
    } else if (difficultyLevel == 3) {
        enemyDifficultyMultiplier = 2.0;
    }

    for (auto& currentEnemyPtr : this->enemyList) 
    {
        currentEnemyPtr->applyDifficultyMultiplier(enemyDifficultyMultiplier);
        currentEnemyPtr->prepareForNewBattle();
    }
}

void Combat::set3DContext(bool is3D, const std::vector<std::string>& matrix, float posX, float posY, float angle, const std::string& title) {
    if (ui) {
        ui->configure3DContext(is3D, matrix, posX, posY, angle, title);
    }
}

void Combat::addLivingAllies(std::vector<std::unique_ptr<Character>> allies)
{
    allyList = std::move(allies);
}

void Combat::addAllyInCombat(std::unique_ptr<Character> ally) {
    allyList.push_back(std::move(ally));
}

Combat::~Combat()
{
    Parry::onUpdateScreen = nullptr;
}

std::string Combat::getCombatTitle() const
{
    std::string title = "EM COMBATE (";
    for (size_t i = 0; i < enemyList.size(); ++i) {
        title += enemyList[i]->getName();
        if (i < enemyList.size() - 1) title += ", ";
    }
    title += ")";
    return title;
}

bool Combat::isPlayerOrAlly(Character* character) const {
    if (character == currentPlayer) return true;
    for (const auto& currentAlly : allyList) {
        if (currentAlly.get() == character) return true;
    }
    return false;
}

std::vector<Character*> Combat::getRawEnemies() const
{
    std::vector<Character*> enemyPointers(enemyList.size());
    std::transform(enemyList.begin(), enemyList.end(), enemyPointers.begin(), [](const std::unique_ptr<Character>& ptr) { return ptr.get(); });
    return enemyPointers;
}

void Combat::displayCombatScreen(bool animateEntry) const
{
    ui->updateStaticScreen(getCombatTitle(), getRawEnemies(), currentPlayer, getLivingAlliesRaw(), animateEntry);
}

std::vector<Character*> Combat::getLivingAlliesRaw() const {
    std::vector<Character*> livingAllies;
    for (const auto& ally : allyList) {
        if (ally->getHealth() > 0) livingAllies.push_back(ally.get());
    }
    return livingAllies;
}

std::string Combat::getFormattedNameWithNumber(Character* c) const {
    if (!c) return "Desconhecido";
    if (c == currentPlayer) return c->getName();
    for (size_t i = 0; i < allyList.size(); ++i) {
        if (allyList[i].get() == c) {
            return c->getName() + " (Aliado " + std::to_string(i + 1) + ")";
        }
    }
    for (size_t i = 0; i < enemyList.size(); ++i) {
        if (enemyList[i].get() == c) {
            return c->getName() + " (" + std::to_string(i + 1) + ")";
        }
    }
    return c->getName();
}

void Combat::prepareCharacterTurn(Character* character) {
    std::string charName = getFormattedNameWithNumber(character);
    registrarLog("");
    registrarLog("=== TURNO " + std::to_string(currentTurnCounter) + " | VEZ DE " + charName + " ===");
    ui->setVisibleTurn(currentTurnCounter, charName);
    character->reduceCooldowns();
    character->processTurnStartEffects();
}

bool Combat::executePlayerOrAllyTurn(Character* character, bool& firstRender, bool processStartEffects) {
    if (processStartEffects) {
        prepareCharacterTurn(character);
    }
    if (character->getHealth() <= 0) return false;

    if (character == currentPlayer) {
        bool cleanedAlly = false;
        for (auto& ally : allyList) {
            if (ally->isMinion() && ally->getHealth() > 0) {
                int damage = std::max(1, static_cast<int>(ally->getMaxHealth() * 0.15));
                ally->modifyHealth(-damage);
                registrarLog(DialogFunctions::formatarMsgStatus(ally->getName() + " perdeu " + std::to_string(damage) + " HP (decomposicao).", Color::MAGENTA));
                
                if (ally->getHealth() <= 0) {
                    registrarLog(DialogFunctions::formatarMsgStatus(ally->getName() + " se decompos durante o combat", Color::RED));
                    cleanedAlly = true;
                }
            }
        }
        // Remove definitivamente da memoria os aliados que morreram pelo dreno
        if (cleanedAlly) {
            allyList.erase(std::remove_if(allyList.begin(), allyList.end(), [](const auto& a) { return a->getHealth() <= 0; }), allyList.end());
        }
    }

    bool turnConsumed = false;
    bool usedInventory = false;

    while (!turnConsumed && character->getHealth() > 0 && !enemyList.empty()) {
        displayCombatScreen(firstRender);
        firstRender = false;
        processPlayerActionMenu(character, turnConsumed, usedInventory);
        
        clearDeadEnemies();
        if (checkWinLossCondition()) return true; 
    }

    if (usedInventory) {
        displayCombatScreen();
        ui->notifyInventoryUnready();
    }
    return false;
}

void Combat::startCombat() 
{
    Parry::onUpdateScreen = [this]() {
        this->displayCombatScreen(false);
    };
    resetAdvancedStatistics();
    currentPlayer->prepareForNewBattle();
    ui->clearFixedMessages();

    for (auto& ally : allyList) {
        ally->prepareForNewBattle();
    }
    ui->animateCombatIntroduction(getCombatTitle(), getRawEnemies(), currentPlayer);

    ui->clearScreen();

    int maxEnemyDexterity = TurnManager::calculateMaxEnemyDexterity(enemyList);
    for (const auto& enemyPtr : enemyList) {
        Bestiary::instance().registerFirstSight(enemyPtr->getRace()->getRaceName());
        Diary::instance().registerRace(enemyPtr->getRace()->getRaceName());
        if (enemyPtr->getClassName() != "Monstro") {
            Diary::instance().registerClass(enemyPtr->getClassName());
        }
    }
    
    bool extraTurnFirstTurn = TurnManager::doesPlayerHaveExtraTurnAtStart(currentPlayer, maxEnemyDexterity);
    bool firstRender = false; // Modificado, pois ja animamos na intro
    
    if (TurnManager::areEnemiesFaster(currentPlayer, maxEnemyDexterity)) {
        displayCombatScreen(firstRender);
        firstRender = false;
        
        if (TurnManager::doEnemiesHaveDoubleAgility(currentPlayer, maxEnemyDexterity)) {
            std::string alert = "A agilidade extrema dos enemies (" + std::to_string(maxEnemyDexterity) + " VS " + std::to_string(currentPlayer->getDexterity()) + ") permite que eles ataquem duas vezes seguidas!";
            InputControl::lerSelecaoMenuEmPopup("ALERTA DE AGILIDADE", {alert}, {"OK"}, Color::RED);

            executeAllEnemiesTurn();
            clearDeadEnemies();
            if (checkWinLossCondition()) return;
            executeAllEnemiesTurn();
            clearDeadEnemies();
            if (checkWinLossCondition()) return;
            
            currentTurnCounter++; // Jogador comeca no Turno 2
        } else {
            ui->notifyEnemiesFaster();
            executeAllEnemiesTurn();
            clearDeadEnemies();
            if (checkWinLossCondition()) return;
        }
    }

    while (currentPlayer->getHealth() > 0 && !enemyList.empty()) {
        // Turno do Jogador
        if (currentPlayer->getHealth() > 0) {
            if (executePlayerOrAllyTurn(currentPlayer, firstRender)) return;

            if (extraTurnFirstTurn && currentTurnCounter == 1) {
                ui->notifyExtraTurn(currentPlayer->getDexterity(), maxEnemyDexterity);
                extraTurnFirstTurn = false;
                if (executePlayerOrAllyTurn(currentPlayer, firstRender, false)) return;
            }
        }
        
        // Turnos dos Aliados
        for (size_t i = 0; i < allyList.size(); ++i) {
            Character* ally = allyList[i].get();
            if (ally->getHealth() <= 0 || enemyList.empty()) continue;
            
            bool isFirstRend = false;
            if (executePlayerOrAllyTurn(ally, isFirstRend)) return;
        }
        
        executeAllEnemiesTurn();
        clearDeadEnemies();
        if (checkWinLossCondition()) return;

        currentTurnCounter++;
    }
}

void Combat::processPlayerActionMenu(Character* actingCharacter, bool& turnConsumed, bool& usedInventoryInTurn)
{
    int chosenAction = ui->getPlayerAction(currentTurnCounter, actingCharacter, getRawEnemies(), currentPlayer, getLivingAlliesRaw());
    
    ui->clearCharacterHUDContext(); // Forca reset visual ao retornar para evitar bugs de persistencia de interface

    switch (chosenAction) 
    {
        case 1: processAttackAction(actingCharacter, turnConsumed); break;
        case 2: processDefendAction(actingCharacter, turnConsumed); break;
        case 3: processAbilityAction(actingCharacter, turnConsumed); break;
        case 4: processInventoryAction(actingCharacter, turnConsumed, usedInventoryInTurn); break;
        case 5: ui->showAttributesScreen(actingCharacter); break;
        case 6: ui->showDiaryScreen(actingCharacter); break;
        case 7: break;
        default: 
            ui->notifyInvalidAction();
            break;
    }
}

void Combat::processAttackAction(Character* actingCharacter, bool& turnConsumed)
{
    std::string weaponName = actingCharacter->getWeapon() ? (" com " + actingCharacter->getWeapon()->getItemName()) : "";

    if (actingCharacter->getAttackType() == AttackType::Area) 
    {
        registrarLog(actingCharacter->getName() + " desferiu um ataque em ÁREA" + weaponName + "!");
        performPhysicalAttack(actingCharacter, nullptr, currentTurnCounter);
        turnConsumed = true;
    }
    else 
    {
        int chosenTargetIndex = ui->getAttackTarget(getCombatTitle(), getRawEnemies(), currentPlayer, getLivingAlliesRaw());
        if (chosenTargetIndex == -1) return;

        Character* target = enemyList[chosenTargetIndex].get();
        std::string targetName = getFormattedNameWithNumber(target);
        registrarLog(actingCharacter->getName() + " iniciou ataque em " + targetName + weaponName + "!");

        performPhysicalAttack(actingCharacter, target, currentTurnCounter);
        turnConsumed = true;
    }
}

Item* Combat::selectShield(Character* actingCharacter) 
{
    std::vector<Item*> shieldList;
    for (auto* item : actingCharacter->getInventory()->getAllItems()) 
    {
        if (item->getType() == EquipmentType::Shield) {
            shieldList.push_back(item);
        }
    }

    if (shieldList.empty()) 
    {
        ui->notifyNoShields(actingCharacter->getName());
        return nullptr;
    }

    int chosenOption = ui->getShieldChoice(actingCharacter->getName(), shieldList);
    return (chosenOption == 0) ? nullptr : shieldList[chosenOption - 1];
}

void Combat::processDefendAction(Character* actingCharacter, bool& turnConsumed)
{
    if (actingCharacter->getDefenseCooldown()) 
    {
        ui->notifyDefenseImbalance(actingCharacter->getName());
        return; 
    }
    
    Item* chosenShield = selectShield(actingCharacter);
    if (chosenShield != nullptr) 
    {
        if (chosenShield->getShieldCurrentDurability() <= 0) {
            std::string alert = "O escudo [" + chosenShield->getItemName() + "] esta quebrado e nao pode ser usado!";
            InputControl::lerSelecaoMenuEmPopup("ESCUDO QUEBRADO", {alert}, {"OK"}, Color::RED);
            return; // Nao consome o turno
        }

        if (!chosenShield->canBeEquippedBy(actingCharacter)) {
            ui->notifyRequirementNotMet(chosenShield->getRequirementMessage());
            return;
        }

        actingCharacter->equipItem(chosenShield);
        actingCharacter->setDefending(true);
        std::string defenseMsg = actingCharacter->getName() + " ASSUME POSTURA DEFENSIVA COM " + chosenShield->getItemName();
        registrarLog(defenseMsg);
        ui->setBannerMessage(defenseMsg, CorBanner::YELLOW);
        ui->notifyDefensiveStance(actingCharacter->getName(), chosenShield->getItemName());
        turnConsumed = true;
    }
}

void Combat::processAbilityAction(Character* actingCharacter, bool& turnConsumed)
{
    std::vector<Character*> rawTargets = getRawEnemies();
    
    actingCharacter->setAbilityCanceled(false);
    std::string abilityMsg = actingCharacter->getName() + " USA HABILIDADE: " + actingCharacter->getClassName();
    registrarLog(abilityMsg);
    ui->setBannerMessage(abilityMsg, CorBanner::YELLOW);
    actingCharacter->getClass()->useClassAbility(this, actingCharacter, rawTargets);
    
    if (actingCharacter->getAbilityCanceled()) return;

    if (actingCharacter->classAbilityConsumesTurn()) turnConsumed = true;
    else {
        // Nao bloqueia thread - deixa o game loop continuar
        InputControl::clearBuffer();
    }
}

void Combat::processInventoryAction(Character* actingCharacter, bool& turnConsumed, bool& usedInventoryInTurn)
{
    int healthBefore = actingCharacter->getHealth();
    bool inventoryConsumed = false;
    
    InventoryCombat::manageInventory(actingCharacter, &inventoryConsumed);
    if (inventoryConsumed) {
        turnConsumed = true;
        usedInventoryInTurn = true;
    }
    
    if (actingCharacter->getHealth() > healthBefore) {
        int healAmount = actingCharacter->getHealth() - healthBefore;
        std::string healMsg = actingCharacter->getName() + " SE CUROU (+" + std::to_string(healAmount) + " HP)";
        registrarLog(healMsg);
        ui->setBannerMessage(healMsg, CorBanner::GREEN_CLARO);
        ui->animateHealOnPlayer(getCombatTitle(), getRawEnemies(), actingCharacter, currentPlayer, getLivingAlliesRaw(), healAmount);
    }

    if (actingCharacter->getItemSelectedForUse() != nullptr) 
    {
        Item* selectedItem = actingCharacter->getItemSelectedForUse();
        
        int chosenTargetIndex = ui->getItemTarget(getCombatTitle(), getRawEnemies(), currentPlayer, getLivingAlliesRaw());

        if (chosenTargetIndex == -1) 
        {
            ui->notifyItemCancelled();
            actingCharacter->setItemSelectedForUse(nullptr);
        } 
        else 
        {
            Character* target = enemyList[chosenTargetIndex].get();
            std::string targetName = getFormattedNameWithNumber(target);
            std::string itemUseMsg = actingCharacter->getName() + " USA " + selectedItem->getItemName() + " EM " + targetName;
            registrarLog(itemUseMsg);
            ui->setBannerMessage(itemUseMsg, CorBanner::GREEN_CLARO);
            
            selectedItem->use(actingCharacter, target);
            
            if (actingCharacter->getQuickConsumable() == selectedItem) {
                actingCharacter->unequipConsumable();
                std::string currentItemName = selectedItem->getItemName();
                for (auto* otherItem : actingCharacter->getInventory()->getAllItems()) {
                    if (otherItem != selectedItem && otherItem->getItemName() == currentItemName) {
                        actingCharacter->equipItem(otherItem);
                        break;
                    }
                }
            }
            
            actingCharacter->getInventory()->removeItem(selectedItem);
            actingCharacter->setItemSelectedForUse(nullptr);
            turnConsumed = true;
            usedInventoryInTurn = true;
            stats_consumedItems++;
        }
    }
}

void Combat::clearDeadEnemies()
{
    for (auto& enemyPtr : enemyList) 
    {
        if (enemyPtr->getHealth() <= 0) 
        {
            int xpBefore = xpEarned;
            int goldBefore = goldEarned;
            size_t itemsBefore = obtainedItems.size();

            std::string deadEnemyName = getFormattedNameWithNumber(enemyPtr.get());
            processEnemyDeath(enemyPtr.get());

            int xpDrop = xpEarned - xpBefore;
            int goldDrop = goldEarned - goldBefore;
            
            std::string deathMsg = deadEnemyName + " E DERROTADO! (+" + std::to_string(xpDrop) + " XP, +" + std::to_string(goldDrop) + " G)";
            registrarLog(deathMsg);
            ui->setBannerMessage(deathMsg, CorBanner::OURO);
            
            std::vector<std::string> deathDrops;
            if (xpDrop > 0) deathDrops.push_back("+" + std::to_string(xpDrop) + " XP");
            if (goldDrop > 0) deathDrops.push_back("+" + std::to_string(goldDrop) + "G");
            
            std::map<std::string, int> itemCounts;
            for (size_t i = itemsBefore; i < obtainedItems.size(); ++i) {
                itemCounts[obtainedItems[i]]++;
            }
            for (auto const& [name, count] : itemCounts) {
                deathDrops.push_back("+" + std::to_string(count) + "x " + name);
            }

            if (!deathDrops.empty()) {
                registrarLog("Recompensas de " + deadEnemyName + ":");
                for (size_t d = 0; d < deathDrops.size(); ++d) {
                    registrarLog("  - " + deathDrops[d]);
                }
            }

            std::vector<Character*> livingAllies = getLivingAlliesRaw();
            ui->animateEnemyDeath(getCombatTitle(), getRawEnemies(), enemyPtr.get(), currentPlayer, livingAllies, deathDrops);
            enemyPtr->setAnimatedDeath(true);
            ui->clearDeadEnemyAndDropsContext();
        }
    }

    enemyList.erase(std::remove_if(enemyList.begin(), enemyList.end(), [](const auto& enemy) { return enemy->getHealth() <= 0; }), enemyList.end());
}

void Combat::executeAllEnemiesTurn() 
{
    ui->clearFixedMessages();
    if (currentPlayer->getSkipEnemyTurn()) 
    {
        // A mensagem na interface foi removida para priorizar o combate limpo
        registrarLog(DialogFunctions::formatarMsgStatus("Os inimigos estao atordoados e nao podem agir!", Color::GREEN));
        currentPlayer->setSkipEnemyTurn(false); 
    }
    else
    {
        std::string enemyTurnText = "═══ TURNO " + std::to_string(currentTurnCounter) + " ║ VEZ DOS INIMIGOS ═══";
        registrarLog("");
        registrarLog(enemyTurnText);
        ui->setVisibleTurn(currentTurnCounter, "INIMIGOS");
        ui->setBannerMessage("VEZ DOS INIMIGOS INICIA - PRESSIONE ENTER", CorBanner::ORANGE);
        displayCombatScreen(false); // Forca o HUD a atualizar o nome do Turno para os enemies antes do ataque iniciar
        InputControl::clearBuffer();
        InputControl::waitForEnter("Vez dos inimigos inicia. Pressione ENTER para continuar...");

        for (size_t i = 0; i < enemyList.size(); ++i) 
        {
            auto& currentEnemyPtr = enemyList[i];
            if (currentPlayer->getHealth() <= 0) break; // Interrompe se o jogador morrer
            
            Character* currentEnemy = currentEnemyPtr.get();
            if (!currentEnemy || currentEnemy->getHealth() <= 0) {
                continue;
            }

            Parry::setAttackingEnemy(currentEnemy);
            currentEnemy->processTurnStartEffects();
            if (currentEnemy->getHealth() <= 0) {
                Parry::setAttackingEnemy(nullptr);
                continue;
            }

            std::string incapacitationReason;
            if (currentEnemy->canAct(incapacitationReason)) 
            {
                // Logica de escolha de alvo do inimigo
                Character* target = EnemyMechanics::selectTarget(getLivingAlliesRaw(), currentPlayer);

                bool turnConsumedByAbility = currentEnemy->getRace()->tryUseActiveAbility(currentEnemy, target, static_cast<int>(currentPlayer->getDifficulty()));
                
                if (!turnConsumedByAbility) {
                    performPhysicalAttack(currentEnemy, target, currentTurnCounter);
                }
            }
            else
            {
                // A mensagem na interface foi removida para priorizar o combate limpo
                registrarLog(DialogFunctions::formatarMsgStatus(currentEnemy->getName() + " esta sob efeito de " + incapacitationReason + " e nao pode agir!", Color::GREEN));
            }
            Parry::setAttackingEnemy(nullptr);
        }
    }

    if (currentPlayer->isDefending())
    {
        currentPlayer->setDefending(false);
        currentPlayer->setDefenseCooldown(true);
    }
    else if (currentPlayer->getDefenseCooldown())
    {
        currentPlayer->setDefenseCooldown(false);
    }

    if (currentPlayer->getCooldownState()) currentPlayer->setCooldownState(false);
    InputControl::clearBuffer();
    ui->setBannerMessage("TURNO DOS INIMIGOS FINALIZADO - PRESSIONE ENTER", CorBanner::OURO);
    InputControl::waitForEnter("Turno dos inimigos finalizado. Pressione ENTER para o seu turno!");
    ui->setBannerMessage("TURNO DO JOGADOR - ESCOLHA UMA ACAO", CorBanner::OURO);
}

void Combat::performPhysicalAttack(Character* attackingCharacter, Character* defendingCharacter, int currentCombatTurn) 
{
    auto [calculatedBaseDamage, piercingDamage] = DamageCalculator::calculateOffensiveBaseDamage(attackingCharacter);

    bool isAttackerPlayerOrAlly = isPlayerOrAlly(attackingCharacter);

    if (isAttackerPlayerOrAlly || static_cast<int>(currentPlayer->getDifficulty()) >= 2) 
    {
        calculatedBaseDamage = attackingCharacter->getRace()->processOffensiveDamage(calculatedBaseDamage, attackingCharacter);
    }

    auto applyDamageCallback = [this, currentCombatTurn](Character* attacker, Character* target, int rawDamage, int piercing) {
        this->applyDamageToTarget(attacker, target, rawDamage, piercing, currentCombatTurn);
    };

    bool applyClassPassive = isAttackerPlayerOrAlly || static_cast<int>(currentPlayer->getDifficulty()) == 3;

    attackingCharacter->getClass()->executeAttackWithClassPassive(attackingCharacter, defendingCharacter, calculatedBaseDamage, piercingDamage, enemyList, applyDamageCallback, applyClassPassive);
}

void Combat::processPostDamage(Character* attacker, Character* target, int finalDamage, bool attemptedParry, bool parrySuccess) {
    std::vector<Character*> livingAllies = getLivingAlliesRaw();

    Parry::setAttackingEnemy(attacker);
    Parry::setParryStatus(0);
    if (attemptedParry) {
        if (parrySuccess) {
            if (finalDamage <= 0) Parry::setParryStatus(1);
            else Parry::setParryStatus(2);
        } else {
            Parry::setParryStatus(3);
        }
    }

    std::string attackDamageMsg = "";
    std::string parryLine2Msg = "";
    CorBanner bannerColor = CorBanner::OURO;

    if (isPlayerOrAlly(attacker)) {
        attackDamageMsg = attacker->getName() + " ATACA " + getFormattedNameWithNumber(target) + " (" + std::to_string(finalDamage) + " DE DANO) - PRESSIONE ENTER";
        bannerColor = CorBanner::YELLOW;
    } else {
        attackDamageMsg = getFormattedNameWithNumber(attacker) + " ATACA JOGADOR (" + std::to_string(finalDamage) + " DE DANO) - PRESSIONE ENTER";
        bannerColor = CorBanner::ORANGE;

        if (attemptedParry) {
            if (parrySuccess) {
                int reflectedDamage = (finalDamage <= 0) ? std::max(1, attacker->getStrength() / 2) : 0;
                Parry::setLastReflectedDamage(reflectedDamage);
                if (finalDamage <= 0) {
                    parryLine2Msg = "PARRY PERFEITO! - DANO REFLETIDO: " + std::to_string(reflectedDamage);
                } else {
                    parryLine2Msg = "PARRY EFETIVO! - DANO REDUZIDO";
                }
            } else {
                parryLine2Msg = "PARRY FALHOU!";
            }
        }
    }
    ui->setBannerMessage(attackDamageMsg, bannerColor, parryLine2Msg);

    if (finalDamage > 0) 
    {
        // ANIMACAO DO DANO NO INIMIGO (Piscar Vermelho + Flicker)
        if (!isPlayerOrAlly(target)) {
            ui->animateDamageOnEnemy(getCombatTitle(), getRawEnemies(), target, attacker, currentPlayer, livingAllies, finalDamage);
        }
        else {
            ui->animateDamageOnPlayer(getCombatTitle(), getRawEnemies(), target, currentPlayer, livingAllies, false, finalDamage);
        }

        // Aplicacao dos efeitos no acerto
        int attackerHealthBefore = attacker->getHealth();
        
        if (attacker->getWeapon()) {
            attacker->getWeapon()->onDealingDamage(attacker, target, finalDamage);
        }
        attacker->getRace()->onDealingDamage(attacker, target, finalDamage);
        
        // Verifica se o atacante se curou (Ex: Passiva da Abominacao)
        if (attacker->getHealth() > attackerHealthBefore) {
            int enemyHeal = attacker->getHealth() - attackerHealthBefore;
            if (!isPlayerOrAlly(attacker)) {
                ui->setBannerMessage(getFormattedNameWithNumber(attacker) + " SE CUROU (" + std::to_string(enemyHeal) + " HP)", CorBanner::GREEN_ESCURO);
                ui->animateHealOnEnemy(getCombatTitle(), getRawEnemies(), attacker, currentPlayer, livingAllies, enemyHeal);
            } else {
                ui->setBannerMessage(attacker->getName() + " SE CUROU (" + std::to_string(enemyHeal) + " HP)", CorBanner::GREEN_CLARO);
                ui->animateHealOnPlayer(getCombatTitle(), getRawEnemies(), attacker, currentPlayer, livingAllies, enemyHeal);
            }
        }
        
        if (target->getArmor() && target->getArmor()->hasProperty(Property::AdaptationArmor)) {
            auto* ef = const_cast<StatusEffect*>(target->findEffect(EffectID::AdaptationWheel));
            if (ef) {
                auto* efWheel = dynamic_cast<AdaptationWheelEffect*>(ef);
                if (efWheel) efWheel->adapt(target, attacker);
            }
        }
    }
    else if (attemptedParry && parrySuccess && isPlayerOrAlly(target)) {
        ui->animateDamageOnPlayer(getCombatTitle(), getRawEnemies(), target, currentPlayer, livingAllies, true, finalDamage);
    } else {
        ui->updateStaticScreen(getCombatTitle(), getRawEnemies(), currentPlayer, livingAllies);
        // Nao bloqueia thread - deixa o game loop continuar
        InputControl::clearBuffer();
    }

    Parry::setParryStatus(0);

    clearDeadEnemies();
}

void Combat::applyDamageToTarget(Character* attackingCharacter, Character* targetCharacter, int rawDamage, int piercingDamage, int /*turnoCombateAtual*/) 
{
    if (Debug::isOneHitKillActive && attackingCharacter == currentPlayer) {
        rawDamage = MAX_DEBUG_DAMAGE;
    }
    if (Debug::isGodModeActive && targetCharacter == currentPlayer) {
        rawDamage = NULL_DAMAGE;
        piercingDamage = NULL_DAMAGE;
    }
    if (targetCharacter->hasEffect(EffectID::Inviolable))
    {
        std::string dodgeMsg = targetCharacter->getName() + " evitou o ataque de " + attackingCharacter->getName();
        registrarLog(DialogFunctions::formatarMsgCombate(dodgeMsg, Color::CYAN));
        
        std::vector<Character*> livingAllies = getLivingAlliesRaw();
        ui->updateStaticScreen(getCombatTitle(), getRawEnemies(), currentPlayer, livingAllies);
        // Nao bloqueia thread - deixa o game loop continuar
        InputControl::clearBuffer();
        return;
    }

    // Logica da Quebra de Resistencia (Po Magico)
    if (attackingCharacter->getWeapon()) attackingCharacter->getWeapon()->beforeDealingDamage(attackingCharacter, targetCharacter);

    int mitigatedBaseDamage = DamageCalculator::calculateDefensiveMitigation(targetCharacter, rawDamage, piercingDamage);
    int parryReducedDamage = 0;
    bool attemptedParry = false;
    bool parryWasSuccessful = false;
    
    bool unstoppableAttack = attackingCharacter && attackingCharacter->getRace()->ignoresParry();

    // Logica do Parry (apenas quando o inimigo ataca o jogador/aliado)
    if (isPlayerOrAlly(targetCharacter) && !isPlayerOrAlly(attackingCharacter) && targetCharacter->isParryEnabled() && !targetCharacter->isDefending()) 
    {
        if (unstoppableAttack) {
            std::string unstoppableMsg = DialogFunctions::formatarMsgCombate(attackingCharacter->getName() + " desfere um ATAQUE IMPARAVEL! O Parry foi ignorado!", Color::FUNDO_RED);
            registrarLog(unstoppableMsg);
            ui->addFixedMessage(ui->combatMargin() + unstoppableMsg + "\n");
        } else {
            attemptedParry = true;
            parryWasSuccessful = Parry::attemptParry(attackingCharacter, targetCharacter, mitigatedBaseDamage, parryReducedDamage);
            stats_attemptedParries++;
            if (parryWasSuccessful) stats_effectiveParries++;
        }
    }

    bool applyPassives = (isPlayerOrAlly(targetCharacter) || static_cast<int>(currentPlayer->getDifficulty()) >= 2);

    DamageResult res = targetCharacter->takeDamage(rawDamage, piercingDamage, parryReducedDamage, attackingCharacter, applyPassives);

    // Logica de adaptacao do Mahoraga ao ter seu ataque bloqueado por escudo
    if (res.blockedDamage > 0 && attackingCharacter->getRaceType() == RaceType::Mahoraga) {
        auto* mahoraga = dynamic_cast<Mahoraga*>(attackingCharacter->getRace());
        if (mahoraga) {
            mahoraga->onAttackBlockedByShield();
        }
    }

    // Contorna o limite de dano minimo caso o aparo absorva todo o impacto
    if (attemptedParry && parryWasSuccessful && parryReducedDamage >= mitigatedBaseDamage) 
    {
        if (targetCharacter == currentPlayer) stats_perfectParries++;
        if (res.finalDamage > 0) 
        {
            targetCharacter->modifyHealth(res.finalDamage); // Restaura o HP retirado pela trava de minimo de damage
            res.finalDamage = 0; // Anula o damage para ativar a Reflexao de Parry Perfeito
        }
    }

    displayAttackResult(attackingCharacter, targetCharacter, res.finalDamage, attemptedParry, parryWasSuccessful, res.blockedDamage, res.shieldBroke, res.brokenShieldName);

    processPostDamage(attackingCharacter, targetCharacter, res.finalDamage, attemptedParry, parryWasSuccessful);

    if (attemptedParry && parryWasSuccessful && res.finalDamage <= 0 && isPlayerOrAlly(targetCharacter) && attackingCharacter) {
        attackingCharacter->getRace()->onSufferingPerfectParry();

        int reflectedDamage = std::max(1, (rawDamage + piercingDamage) / 2);
        attackingCharacter->modifyHealth(-reflectedDamage);
        std::string reflectionAttacker = getFormattedNameWithNumber(attackingCharacter);
        
        std::string reflectionMsg = "PARRY PERFEITO! " + getFormattedNameWithNumber(targetCharacter) + " refletiu " + std::to_string(reflectedDamage) + " de dano de volta em " + reflectionAttacker + "!";
        registrarLog(reflectionMsg);
        
        std::vector<Character*> livingAllies = getLivingAlliesRaw();
        if (!isPlayerOrAlly(attackingCharacter)) {
            ui->animateDamageOnEnemy(getCombatTitle(), getRawEnemies(), attackingCharacter, targetCharacter, currentPlayer, livingAllies, reflectedDamage);
            totalDamageDealt += reflectedDamage;
        } else {
            ui->animateDamageOnPlayer(getCombatTitle(), getRawEnemies(), attackingCharacter, currentPlayer, livingAllies, false, reflectedDamage);
        }
    }
}

void Combat::displayAttackResult(Character* attacker, Character* target, int finalDamage, bool attemptedParry, bool parrySuccess, int blockedDamage, bool shieldBroke, const std::string& brokenShieldName)
{
    bool isPlayerOrAllyTarget = isPlayerOrAlly(target);
    std::string attackerName = getFormattedNameWithNumber(attacker);
    std::string targetName = getFormattedNameWithNumber(target);

    if (blockedDamage > 0) {
        std::string defenseMsg = "O escudo de " + targetName + " bloqueou " + std::to_string(blockedDamage) + " de dano do ataque de " + attackerName + "!";
        registrarLog(defenseMsg);
        
        if (shieldBroke) {
            std::string breakMsg = "ALERTA: O escudo [" + brokenShieldName + "] de " + targetName + " FOI DESTRUIDO pelo impacto!";
            registrarLog(breakMsg);
            target->unequipShield();
        }
    }

    if (isPlayerOrAllyTarget) 
    {
        if (attemptedParry) {
            std::string parryLogMsg = Parry::getFeedbackMessage(parrySuccess, finalDamage);
            registrarLog(targetName + " realizou PARRY contra " + attackerName + ": " + parryLogMsg);
        }
        else if (finalDamage > 0) 
        {
            registrarLog(attackerName + " atacou " + targetName + " e causou " + std::to_string(finalDamage) + " de dano!");
        }
        else if (finalDamage == 0 && target->isDefending()) 
        {
            registrarLog("O dano do ataque de " + attackerName + " foi totalmente absorvido pela defesa de " + targetName + "!");
        }
        
        if (finalDamage > 0 && target == currentPlayer) totalDamageTaken += finalDamage;
    }
    else 
    {
        if (finalDamage > 0) {
            registrarLog(attackerName + " atacou " + targetName + " e causou " + std::to_string(finalDamage) + " de dano!");
            if (finalDamage > stats_highestDamageDealt) stats_highestDamageDealt = finalDamage;
            if (target != currentPlayer) totalDamageDealt += finalDamage;
        } else if (finalDamage == 0 && target->isDefending()) {
            registrarLog(targetName + " bloqueou completamente o dano de " + attackerName + "!");
        }
    }
}

bool Combat::checkWinLossCondition() 
{
    bool isVictory = enemyList.empty();
    bool isDefeat = currentPlayer->getHealth() <= 0;

    if (isVictory || isDefeat) 
    { 
        currentPlayer->clearEffects(); // Remove buffs e debuffs ao final da batalha
        if (isVictory) {
            ui->showVictoryScreen(currentPlayer, goldEarned, xpEarned, totalDamageDealt, 
                                totalDamageTaken, currentPlayer->getTotalHealingReceived(), currentTurnCounter, 
                                obtainedItems, defeatedEnemies, stats_perfectParries, stats_highestDamageDealt, stats_attemptedParries, stats_effectiveParries, stats_consumedItems, stats_newDiscoveries);
        } else {
            ui->showDefeatScreen(currentPlayer, goldEarned, xpEarned, totalDamageDealt, totalDamageTaken, currentPlayer->getTotalHealingReceived(), currentTurnCounter); 
        }
        currentPlayer->finishBattle();
        return true; 
    }
    return false;
}

void Combat::processEnemyDeath(Character* enemy)
{
    registrarLog(DialogFunctions::formatarMsgCombate(enemy->getName() + " derrotado!", Color::RED));
    defeatedEnemies.push_back(enemy->getName());

    std::string raceName = enemy->getRace()->getRaceName();
    if (!Bestiary::instance().isDefeated(raceName)) {
        stats_newDiscoveries.push_back("Novo monstro catalogado: " + raceName);
    }

    Bestiary::instance().registerDefeat(enemy->getRace()->getRaceName());

    if (enemy->getName() == "Mahoraga") {
        Progression::instance().setFlag(Flags::Forest_MahoragaDefeated, true);
    }

    // Passiva do Necromancer: Coletar alma
    if (currentPlayer->getClassType() == ClassType::Necromancer) {
        currentPlayer->addSoul(enemy->clone());
        std::string msg = DialogFunctions::formatarMsgHabilidade("Voce coletou a alma de " + enemy->getName() + "!", Color::MAGENTA);
        registrarLog(msg);
    }

    registrarLog("═══ DROPS ═══", Color::YELLOW);

    size_t itemsBefore = obtainedItems.size();
    enemy->executeDrops(currentPlayer, obtainedItems, goldEarned, xpEarned);
    for (size_t i = itemsBefore; i < obtainedItems.size(); ++i) {
        if (!Bestiary::instance().hasCollectedDrop(raceName, obtainedItems[i])) {
            stats_newDiscoveries.push_back("Novo drop descoberto: " + obtainedItems[i]);
        }
        Bestiary::instance().registerDrop(enemy->getRace()->getRaceName(), obtainedItems[i]);
    }
}
