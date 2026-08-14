#include "Warrior.h"

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include "../../../systems/combat/Combat.h"
#include "../../../systems/inventory/ItemFactory.h"
#include "../../../core/utils/DialogFunctions.h"
#include "../../../ui/screens/combat/ScreenCombat.h"
#include "../../../core/utils/Color.h"

// --- INFORMAÇÕES DA CLASSE ---
std::string Warrior::getClassName() const 
{ 
    return "Knight"; 
}

const std::vector<std::string>& Warrior::getClassMenuAppearance() const 
{
    static const std::vector<std::string> appearance;
    return appearance;
}

Attributes Warrior::getClassAttributes() const
{
    return { 0, 20, 10, 5, 10, 5, 5 };
}

std::vector<std::unique_ptr<Item>> Warrior::getClassEquipment() const 
{
    auto equipment = ItemFactory::criarKitPocoes();

    equipment.push_back(ItemFactory::criarItem(ItemID::EspadaFerro));
    equipment.push_back(ItemFactory::criarItem(ItemID::EscudoMetal));
    equipment.push_back(ItemFactory::criarItem(ItemID::ArmaduraMalha));
    return equipment;
}

// --- PASSIVA DA CLASSE ---
std::string Warrior::getClassPassiveName() const 
{ 
    return "Golpe decisivo"; 
}

std::string Warrior::getClassPassiveDescription() const 
{ 
    return "Causa +10%/+20%/+30% de damage em enemies com menos de 30%/20%/10% de HP."; 
}

// --- HABILIDADE DA CLASSE ---
std::string Warrior::getClassAbilityCooldownDescription() const 
{ 
    return "Recarga: 3 turnos."; 
}

std::string Warrior::getClassAbilityName() const 
{ 
    return "Grito de guerra"; 
}

std::string Warrior::getClassAbilityDescription() const 
{ 
    return "Gasta seu turno para aumentar Forca e Destreza em 1.5x por 2 turnos."; 
}

void Warrior::useClassAbility(Combat* /*combat*/, Character* userCharacter, std::vector<Character*>& /*enemyList*/) 
{
    int remainingTurns = userCharacter->obterRecargaHabilidade(AbilityID::Determination);
    if (checkAndReportCooldown(userCharacter, remainingTurns, getClassAbilityName())) return;

    if (userCharacter->possuiEfeito(EffectID::WarCry)) {
        std::string msg = DialogFunctions::formatarMsgSistema("A habilidade " + getClassAbilityName() + " ja esta ativa!", Color::YELLOW);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        userCharacter->definirHabilidadeCancelada(true);
        return;
    }

    int strengthBonus = userCharacter->getStrength() / 2;
    int dexterityBonus = userCharacter->getDexterity() / 2;
    
    userCharacter->adicionarEfeito(std::make_unique<WarCryEffect>(2, strengthBonus, dexterityBonus));
    userCharacter->definirCooldown(AbilityID::Determination, 4);
    
    std::string msg = DialogFunctions::formatarMsgHabilidade("Grito de guerra! Forca +" + std::to_string(strengthBonus) + " e Destreza +" + std::to_string(dexterityBonus) + "!");
    notifyCombatMessage(msg, msg);
}

// --- PROCESSAMENTO DE DANO ---
int Warrior::processPreAttackDamage(Character* /*attacker*/, Character* defender, int baseDamage, bool /*isAttackerPlayer*/, size_t /*enemyCount*/) {
    int finalDamage = baseDamage;
    
    if (!defender) return finalDamage;

    double percHealth = (double)defender->obterVida() / defender->obterVidaMaxima();
    int bonus = 0;
    std::string state = "";
    
    if (percHealth < 0.10) { bonus = 30; state = "nas ultimas"; }
    else if (percHealth < 0.20) { bonus = 20; state = "gravemente ferido"; }
    else if (percHealth < 0.30) { bonus = 10; state = "ferido"; }
    
    if (bonus > 0) {
        finalDamage = static_cast<int>(finalDamage * (1.0 + bonus / 100.0));
        std::string logText = DialogFunctions::formatarMsgHabilidade("Golpe Decisivo: O enemy esta " + state + "! Damage aumentado em " + std::to_string(bonus) + "%!", Color::RED);
        notifyCombatMessage(logText, logText);
    }
    
    return finalDamage;
}
