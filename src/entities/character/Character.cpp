#include "Character.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <cassert>

#include "../classes/ClassBase.h"
#include "../races/RaceBase.h"
#include "../classes/necro-clone/NecroClone.h"
#include "../../core/utils/Constants.h"
#include "../../ui/screens/combat/ScreenCombat.h"

std::unordered_set<Character*> Character::activeCharacters;

bool Character::isValid(Character* p) {
    return activeCharacters.find(p) != activeCharacters.end();
}

Character::Character(const Character& other)
    : characterName(other.characterName),
      currentHealth(other.currentHealth),
      race(std::make_unique<RaceClone>(other.race ? other.race->getRaceName() : "Desconhecido", other.race ? other.race->getRaceAppearance() : std::vector<std::string>())),
      characterClass(std::make_unique<PlayerClassClone>()),
      finalStats(other.finalStats),
      inventory(std::make_unique<Inventory>()),
      itemSelectedForUse(nullptr),
      levelSystem(std::make_unique<LevelSystem>(other.levelSystem->getLevel(), other.levelSystem->getCurrentXp(), other.levelSystem->getXpToLevelUp()))
{
    system = other.system;
    
    combat.isDefending = other.combat.isDefending;
    // collectedSouls nao sao copiadas
    combat.defenseCooldown = other.combat.defenseCooldown;
    combat.abilityCooldown = other.combat.abilityCooldown;
    combat.skipEnemyTurn = other.combat.skipEnemyTurn;
    combat.abilityCanceled = other.combat.abilityCanceled;
    combat.animatedDeath = other.combat.animatedDeath;
    combat.currentMultiplier = other.combat.currentMultiplier;
    combat.totalHealingReceived = other.combat.totalHealingReceived;
    combat.fixedMaxHealth = other.combat.fixedMaxHealth;
    combat.activeCooldowns = other.combat.activeCooldowns;

    cache_ = other.cache_;
    activeCharacters.insert(this);

    // Copia dos Itens (Conforme regra: "mas possui os mesmos itens")
    for (const auto& pair : other.equipment) {
        if (pair.second) {
            auto itemCopy = ItemFactory::criarItem(pair.second->getNameItem());
            if (itemCopy) { 
                this->equipment[pair.first] = itemCopy.get(); 
                this->inventory->adicionarItem(std::move(itemCopy)); 
            }
        }
    }
    updateCacheIfNeeded();
}

Character::Character(const std::string& name, std::unique_ptr<RaceBase> chosenRace, std::unique_ptr<ClassBase> chosenClass)
    : characterName(name),
      currentHealth(0),
      race(std::move(chosenRace)),
      characterClass(std::move(chosenClass)),
      finalStats{ 0, 0, 0, 0, 0, 0, 0 },
      inventory(std::make_unique<Inventory>()),
      itemSelectedForUse(nullptr),
      levelSystem(std::make_unique<LevelSystem>(1, 0, Constants::BASE_XP_TO_LEVEL_UP))
{
    auto receiveAndEquipKit = [this](std::vector<std::unique_ptr<Item>> kit) {
        for (auto& itemUnique : kit) {
            Item* ptr = itemUnique.get();
            this->inventory->adicionarItem(std::move(itemUnique)); 
            this->equipItem(ptr);            
        }
    };

    receiveAndEquipKit(this->characterClass->getClassEquipment());
    receiveAndEquipKit(this->race->getRaceEquipment());

    calculateAttributes();
    activeCharacters.insert(this);
}

Character::~Character() 
{
    activeCharacters.erase(this);
}  

std::unique_ptr<Character> Character::clone() const {
    return std::make_unique<Character>(*this);
}

void Character::scaleAttributes(double factor) {
    finalStats.scaleAll(factor);
    combat.fixedMaxHealth = finalStats.health; 
    forceCacheRecalculation();
    currentHealth = getMaxHealth();
}

void Character::addSoul(std::unique_ptr<Character> soul) { combat.collectedSouls.push_back(std::move(soul)); }

std::vector<std::unique_ptr<Character>>& Character::getSouls() { return combat.collectedSouls; }

size_t Character::getSoulCount() const { return combat.collectedSouls.size(); }

std::unique_ptr<Character> Character::removeSoul(int index) {
    if (index < 0 || index >= static_cast<int>(combat.collectedSouls.size())) return nullptr;
    auto soul = std::move(combat.collectedSouls[index]);
    combat.collectedSouls.erase(combat.collectedSouls.begin() + index);
    return soul;
}

int* Character::getStaticAttributePointer(AttributeType attribute) {
    switch (attribute) {
        case AttributeType::Strength: return &finalStats.strength;
        case AttributeType::Dexterity: return &finalStats.dexterity;
        case AttributeType::Resistance: return &finalStats.resistance;
        case AttributeType::Constitution: return &finalStats.constitution;
        case AttributeType::Intelligence: return &finalStats.intelligence;
        case AttributeType::Wisdom: return &finalStats.wisdom;
        default: return nullptr;
    }
}

bool Character::levelUp(AttributeType attribute)
{
    if (levelSystem->getCurrentXp() < levelSystem->getXpToLevelUp()) return false;

    if (attribute == AttributeType::Health) {
        finalStats.health += Constants::HEALTH_GAIN_PER_LEVEL;
        currentHealth += Constants::HEALTH_GAIN_PER_LEVEL;
    } else if (int* attr = getStaticAttributePointer(attribute)) {
        *attr += Constants::STAT_GAIN_PER_LEVEL;
    } else {
        return false;
    }

    levelSystem->setCurrentXp(levelSystem->getCurrentXp() - levelSystem->getXpToLevelUp());
    levelSystem->setXpToLevelUp(static_cast<int>(std::min(levelSystem->getXpToLevelUp() * Constants::XP_MULTIPLIER_PER_LEVEL, Constants::MAX_XP)));
    levelSystem->setLevel(levelSystem->getLevel() + 1);
    cache_.dirty = true;
    return true;
}

void Character::modifyStaticAttribute(AttributeType attribute, int value)
{
    if (int* attr = getStaticAttributePointer(attribute)) {
        *attr = std::max(0, *attr + value);
        cache_.dirty = true;
    }
}

void Character::reduceCooldowns()
{
    if (combat.defenseCooldown) combat.defenseCooldown = false;
    if (combat.abilityCooldown) combat.abilityCooldown = false;
    if (combat.activeCooldowns.empty()) return;
    for (auto& pair : combat.activeCooldowns)
    {
        if (pair.second > 0) pair.second--;
    }
}

void Character::prepareForNewBattle()
{
    combat.reset();
    combat.fixedMaxHealth = getMaxHealth();
    clearEffects();
    
    if (getArmor() && getArmor()->hasProperty(Property::AdaptationArmor)) {
        addEffect(std::make_unique<AdaptationWheelEffect>());
    }
}

void Character::calculateAttributes()
{
    this->finalStats.addAttributes(race->getRaceAttributes());
    this->finalStats.addAttributes(characterClass->getClassAttributes());
    this->currentHealth = getMaxHealth();
    cache_.dirty = true;
}

void Character::updateCacheIfNeeded() const {
    std::lock_guard<std::mutex> lock(mutexCache_);
    if (!cache_.dirty) return;
    
    double mult = system.difficultyMultiplier;
    auto applyMult = [mult](int val) { return static_cast<int>(val * mult); };

    cache_.maxHealth = applyMult(finalStats.health);
    cache_.strength = applyMult(finalStats.strength);
    cache_.resistance = applyMult(finalStats.resistance);
    cache_.constitution = applyMult(finalStats.constitution);
    cache_.intelligence = applyMult(finalStats.intelligence);
    cache_.wisdom = applyMult(finalStats.wisdom);

    int penalty = getArmor() ? (getArmor()->obterReducaoFixa() / 3) : 0;
    if (getArmor() && getArmor()->getNameItem() == "Armor de bau") penalty = 10;
    if (characterClass) penalty = characterClass->processArcherPassiveArmorPenalty(penalty);
    
    int baseDexterity = static_cast<int>(finalStats.dexterity * mult);
    int finalDexterity = baseDexterity - penalty;
    cache_.dexterity = finalDexterity > 0 ? finalDexterity : 0;

    int armorBonus = getArmor() ? getArmor()->obterReducaoFixa() : 0;
    int reduction = cache_.resistance + armorBonus;
    
    double percentageReduction = cache_.constitution / 100.0;
    if (percentageReduction > 0.50) percentageReduction = 0.50;
    cache_.percentageReduction = static_cast<int>(reduction * (1.0 - percentageReduction));

    cache_.dirty = false;
}

void Character::setMultiplier(double newMultiplier) 
{ 
    if (characterClass) {
        combat.currentMultiplier = characterClass->processBardPassiveBuffMultiplier(newMultiplier);
    } else {
        combat.currentMultiplier = newMultiplier;
    }
}

void Character::applyDifficultyMultiplier(double mult)
{
    if (mult <= 1.0) return;
    system.difficultyMultiplier = mult;
    cache_.dirty = true;
    this->currentHealth = getMaxHealth();
}

void Character::modifyHealth(int value) 
{
    if (value > 0 && characterClass != nullptr) {
        value = characterClass->processBardPassiveHealing(value);
    }

    int healthBefore = this->currentHealth;
    this->currentHealth = std::clamp(this->currentHealth + value, 0, getMaxHealth());

    if (this->currentHealth > healthBefore) 
    {
        combat.totalHealingReceived += (this->currentHealth - healthBefore);
    }
}

const StatusEffect* Character::findEffect(EffectID id) const {
    auto it = std::find_if(activeEffects.begin(), activeEffects.end(), [id](const auto& ef) {
        return ef->obterID() == id;
    });
    return it != activeEffects.end() ? it->get() : nullptr;
}

bool Character::hasEffect(EffectID id) const {
    return findEffect(id) != nullptr;
}

int Character::getEffectTurns(EffectID id) const {
    const StatusEffect* ef = findEffect(id);
    return ef ? ef->obterTurnosRestantes() : 0;
}

void Character::showStatus() const 
{
}

std::string Character::getClassName() const 
{
    return this->characterClass->getClassName();
}

ClassType Character::getClassType() const 
{
    if (this->characterClass) return this->characterClass->getClassType();
    return ClassType::None;
}

RaceType Character::getRaceType() const 
{
    if (this->race) return this->race->getRaceType();
    return RaceType::None;
}

void Character::equipItem(Item* item)
{
    if (item == nullptr) return;
    if (item->getType() == EquipmentType::Weapon) this->equipment[EquipmentSlot::MainHand] = item;
    else if (item->getType() == EquipmentType::Shield) this->equipment[EquipmentSlot::OffHand] = item;
    else if (item->getType() == EquipmentType::Armor)
    {
        this->equipment[EquipmentSlot::Armor] = item;
        if (combat.fixedMaxHealth > 0 && item->hasProperty(Property::AdaptationArmor)) {
            if (!hasEffect(EffectID::AdaptationWheel)) {
                addEffect(std::make_unique<AdaptationWheelEffect>());
            }
        }
    }
    else if (item->getType() == EquipmentType::Consumable) this->equipment[EquipmentSlot::Consumable] = item;
    cache_.dirty = true;
}

RaceBase* Character::getRace() const 
{
    return this->race.get();
}

ClassBase* Character::getClass() const 
{
    return this->characterClass.get();
}

AttackType Character::getAttackType() const 
{
    if (this->characterClass) return this->characterClass->getAttackType();
    return AttackType::Single;
}

bool Character::classAbilityConsumesTurn() const 
{
    if (this->characterClass) return this->characterClass->abilityConsumesTurn();
    return true;
}

int Character::calculateBaseDefense(int rawDamage, int piercingDamage) {
    int damageWithoutPiercing = std::max(0, rawDamage - piercingDamage);

    updateCacheIfNeeded();

    int finalDamage = static_cast<int>(damageWithoutPiercing - cache_.percentageReduction);
    if (finalDamage < 1 && damageWithoutPiercing > 0) finalDamage = 1;
    else if (damageWithoutPiercing == 0) finalDamage = 0;

    return finalDamage + piercingDamage;
}

DamageResult Character::takeDamage(int rawDamage, int piercingDamage, int parryReducedDamage, IAttacker* attacker, bool applyPassives) {
    DamageResult result;

    int finalDamage = calculateBaseDefense(rawDamage, piercingDamage);

    for (auto& ef : activeEffects) {
        finalDamage = ef->processarDanoRecebido(finalDamage);
    }

    finalDamage = std::max(0, finalDamage - parryReducedDamage);

    if (combat.isDefending && getShield() != nullptr) {
        Item* shield = getShield();
        result.blockedDamage = shield->obterReducaoDanoFixaEscudo();
        finalDamage = std::max(0, finalDamage - result.blockedDamage);

        shield->reduzirDurabilidade(1);
        if (shield->obterDurabilidadeAtualEscudo() <= 0) {
            result.shieldBroke = true;
            result.brokenShieldName = shield->getNameItem();
            inventory->removerItem(shield);
            unequipShield();
        }
    }

    if (applyPassives && race) finalDamage = race->processDefensiveDamage(finalDamage, this);
    
    if (attacker) finalDamage = attacker->ensureMinimumDamage(finalDamage);

    if (finalDamage > 0) modifyHealth(-finalDamage);

    result.finalDamage = finalDamage;
    return result;
}

void Character::addEffect(std::unique_ptr<StatusEffect> effect) {
    effect->aoEntrar(this);
    if (processingEffects) {
        effectAdditionQueue.push_back(std::move(effect));
    } else {
        activeEffects.push_back(std::move(effect));
    }
    cache_.dirty = true;
}

void Character::processTurnStartEffects() {
    processingEffects = true;
    for (auto& ef : activeEffects) {
        ef->aplicarInicioTurno(this);
        ef->decrementarTurno();
    }

    activeEffects.erase(
        std::remove_if(activeEffects.begin(), activeEffects.end(),
            [this](const std::unique_ptr<StatusEffect>& ef) {
                if (ef->expirou()) {
                    ef->aoSair(this);
                    cache_.dirty = true;
                    return true;
                }
                return false;
            }),
        activeEffects.end()
    );
    processingEffects = false;

    for (EffectID id : effectRemovalQueue) {
        removeEffect(id);
    }
    effectRemovalQueue.clear();

    for (auto& ef : effectAdditionQueue) {
        activeEffects.push_back(std::move(ef));
    }
    effectAdditionQueue.clear();
}

void Character::clearEffects() {
    for (auto& ef : activeEffects) {
        ef->aoSair(this); // Garante que os atributos (como Forca e Destreza) sejam restaurados
    }
    activeEffects.clear();
    effectAdditionQueue.clear();
    effectRemovalQueue.clear();
    cache_.dirty = true;
}

void Character::removeEffect(EffectID id) {
    if (processingEffects) {
        effectRemovalQueue.push_back(id);
        return;
    }
    auto it = std::find_if(activeEffects.begin(), activeEffects.end(),
        [id](const std::unique_ptr<StatusEffect>& ef) {
            return ef->obterID() == id;
        });
    if (it != activeEffects.end()) {
        (*it)->aoSair(this);
        activeEffects.erase(it);
        cache_.dirty = true;
    }
}

bool Character::canAct(std::string& outIncapacityReason) const {
    auto it = std::find_if(activeEffects.begin(), activeEffects.end(), [](const auto& ef) {
        return ef->impedeAcao();
    });
    if (it != activeEffects.end()) {
        outIncapacityReason = (*it)->getName();
        return false;
    }
    return true;
}

void Character::getActiveEffectIDs(std::vector<EffectID>& outIDs) const {
    outIDs.clear();
    outIDs.reserve(activeEffects.size());
    std::transform(activeEffects.begin(), activeEffects.end(), std::back_inserter(outIDs), [](const auto& ef) {
        return ef->obterID();
    });
}

void Character::executeDrops(Character* currentPlayer, std::vector<std::string>& obtainedItems, int& totalGold, int& totalXp) {
    if (race) {
        race->performDrops(this, currentPlayer, obtainedItems, totalGold, totalXp);
    }
}

int Character::ensureMinimumDamage(int currentDamage) {
    if (getWeapon()) {
        return getWeapon()->garantirDanoMinimo(currentDamage);
    }
    return currentDamage;
}

std::pair<int, int> Character::calculateBaseOffensiveDamage() {
    double attributeMultiplier = getMultiplier();

    int weaponPhysicalDamage = 1;
    int weaponMagicalDamage = 0;
    int currentPiercing = 0;

    if (getWeapon()) 
    {
        weaponPhysicalDamage = getWeapon()->obterDanoFisico();
        weaponMagicalDamage = getWeapon()->obterDanoMagico();

        if (getWeapon()->hasProperty(Property::Magic)) {
            int magicBonus = weaponPhysicalDamage / 2;
            double scaledBonus = magicBonus * (1.0 + (getWisdom() / 100.0));
            currentPiercing = static_cast<int>(scaledBonus * attributeMultiplier);
        }
    }

    int effectiveStrength = getStrength();
    int effectiveDexterity = getDexterity();
    int effectiveIntelligence = getIntelligence();
    int effectiveWisdom = getWisdom();

    if (weaponPhysicalDamage == 0 && weaponMagicalDamage > 0) {
        effectiveStrength /= 10; effectiveDexterity /= 10;
    } else if (weaponPhysicalDamage > 0 && weaponMagicalDamage == 0) {
        effectiveIntelligence /= 10; effectiveWisdom /= 10;
    }

    int calculatedPhysicalDamage = std::max(0, static_cast<int>((weaponPhysicalDamage + effectiveStrength) * (1.0 + (effectiveDexterity / 100.0))));
    int calculatedMagicalDamage = std::max(0, static_cast<int>((weaponMagicalDamage + effectiveIntelligence) * (1.0 + (effectiveWisdom / 100.0))));
    
    int total = std::max(1, calculatedPhysicalDamage + calculatedMagicalDamage);
    int finalTotal = static_cast<int>(total * attributeMultiplier);
    int finalPiercing = currentPiercing;

    if (getWeapon() && getWeapon()->hasProperty(Property::IgnoreDefense)) {
        finalPiercing = finalTotal;
    }

    if (race && race->ignoresShield()) {
        finalPiercing = finalTotal;
    }

    if (hasEffect(EffectID::TrueAim)) {
        finalTotal *= 2;
        finalPiercing *= 2;
        removeEffect(EffectID::TrueAim);
    }

    return { finalTotal, finalPiercing };
}

void Character::finishBattle() { 
    combat.fixedMaxHealth = 0; 
    if (system.hasTrollRegeneration && currentHealth > 0 && currentHealth < getMaxHealth()) {
        modifyHealth(getMaxHealth());
        InputControl::aguardarEnter();
    }
}

bool Character::isBoss() const {
    RaceType t = getRaceType();
    return (t == RaceType::Mahoraga || 
            t == RaceType::ExiledOrc || 
            t == RaceType::Troll || 
            t == RaceType::ForestAbomination);
}

bool Character::isInCombat() const {
    return false; 
}

void Character::enterCombat() {
}

void Character::exitCombat() {
}

void Character::prepareForCombat() {
}
