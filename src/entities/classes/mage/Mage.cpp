#include "Mage.h"

#include <iostream>
#include <memory>

#include "../../../systems/combat/Combat.h"
#include "../../../systems/inventory/ItemFactory.h"
#include "../../../core/utils/DialogFunctions.h"
#include "../../../ui/screens/combat/ScreenCombat.h"
#include "../../../core/utils/Color.h"

// Informacoes da classe
std::string Mage::getClassName() const 
{
     return "Mage"; 
}

const std::vector<std::string>& Mage::getClassMenuAppearance() const 
{
    static const std::vector<std::string> appearance;
    return appearance;
}

Attributes Mage::getClassAttributes() const
{
    return { 0, 5, 5, 3, 10, 15, 15 };
}

std::vector<std::unique_ptr<Item>> Mage::getClassEquipment() const 
{
    auto equipment = ItemFactory::criarKitPocoes();

    equipment.push_back(ItemFactory::criarItem(ItemID::CajadoCristal));
    equipment.push_back(ItemFactory::criarItem(ItemID::BarreiraMagica));
    equipment.push_back(ItemFactory::criarItem(ItemID::Tunica));
    return equipment;
}

// Passiva da classe
std::string Mage::getClassPassiveName() const 
{ 
    return "Foco arcano"; 
}

std::string Mage::getClassPassiveDescription() const 
{ 
    return "Ataques ressoam (25% em area) ou causam +25% de damage em alvo unico."; 
}

// Habilidade da classe
std::string Mage::getClassAbilityCooldownDescription() const 
{ 
    return "Recarga: 3 turnos."; 
}

std::string Mage::getClassAbilityName() const 
{ 
    return "Canalizacao arcana"; 
}

std::string Mage::getClassAbilityDescription() const 
{ 
    return "Pula seu turno para se defender e dobra o damage no proximo turno. Recarga: 3 turnos."; 
}

void Mage::useClassAbility(Combat* /*combate*/, Character* userCharacter, std::vector<Character*>& /*listaInimigos*/) 
{
    int remainingTurns = userCharacter->obterRecargaHabilidade(AbilityID::ArcaneChanneling);
    if (checkAndReportCooldown(userCharacter, remainingTurns, getClassAbilityName())) return;
    
    userCharacter->definirMultiplicador(2.0);
    userCharacter->adicionarEfeito(std::make_unique<AttributeBuffEffect>(2)); 
    userCharacter->definirCooldown(AbilityID::ArcaneChanneling, 4);
    
    Item* shield = userCharacter->obterEscudo();
    if (shield) {
        userCharacter->definirDefendendo(true);
        std::string msg = DialogFunctions::formatarMsgHabilidade("Canalizacao arcana! Defendendo com " + shield->getNameItem() + "! 2x Damage no prox. ataque!");
        notifyCombatMessage(msg, msg);
    } else {
        std::string msg = DialogFunctions::formatarMsgHabilidade("Canalizacao arcana! Foco magico para 2x Damage no proximo ataque!");
        notifyCombatMessage(msg, msg);
    }
}

// Processamento de dano
int Mage::processPreAttackDamage(Character* /*atacante*/, Character* defender, int baseDamage, bool isAttackerPlayer, size_t enemyCount) {
    if (defender == nullptr) return baseDamage;
    if (!isAttackerPlayer || enemyCount <= 1) {
        int increasedDamage = static_cast<int>(baseDamage * 1.25);
        std::string logMsg = DialogFunctions::formatarMsgHabilidade("Foco Arcano: Damage concentrado aumentado em 25%!", Color::MAGENTA);
        notifyCombatMessage(logMsg, logMsg);
        return increasedDamage;
    }
    return baseDamage;
}

void Mage::processPostAttackDamage(Character* attacker, Character* currentTarget, Character* mainDefender, int baseDamage, int piercingDamage, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool isAttackerPlayer, bool isArea, bool& triggeredPassive) {
    if (isAttackerPlayer && !isArea && currentTarget != mainDefender && currentTarget->obterVida() > 0) {
        if (!triggeredPassive) {
            int areaDmgMsg = static_cast<int>(baseDamage * 0.25);
            std::string logMsg = DialogFunctions::formatarMsgHabilidade("Foco Arcano: A magia ressoa, causando " + std::to_string(areaDmgMsg) + " de damage aos enemies proximos!", Color::MAGENTA);
            notifyCombatMessage(logMsg, logMsg);
            triggeredPassive = true;
        }
        int areaDamage = static_cast<int>(baseDamage * 0.25);
        int areaPiercing = static_cast<int>(piercingDamage * 0.25);
        applyDamage(attacker, currentTarget, areaDamage, areaPiercing);
    }
}
