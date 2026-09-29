#include "Archer.h"

#include <iostream>
#include <memory>

#include "../../../systems/combat/Combat.h"
#include "../../../systems/inventory/ItemFactory.h"
#include "../../../core/utils/DialogFunctions.h"

// Informacoes da classe
std::string Archer::getClassName() const 
{
     return "Archer"; 
}

const std::vector<std::string>& Archer::getClassMenuAppearance() const 
{
    static const std::vector<std::string> appearance;
    return appearance;
}

Attributes Archer::getClassAttributes() const
{
    return { 0, 10, 20, 3, 10, 5, 5 };
}

std::vector<std::unique_ptr<Item>> Archer::getClassEquipment() const 
{
    auto equipment = ItemFactory::criarKitPocoes();

    equipment.push_back(ItemFactory::criarItem(ItemID::ArcoMadeira));
    equipment.push_back(ItemFactory::criarItem(ItemID::BracedeirasPrata));
    equipment.push_back(ItemFactory::criarItem(ItemID::ArmaduraCouro));
    return equipment;
}

// Passiva da classe
std::string Archer::getClassPassiveName() const 
{ 
    return "Passos leves"; 
}

std::string Archer::getClassPassiveDescription() const 
{ 
    return "Penalidade de armaduras e debuffs de lentidao reduzidos pela metade."; 
}

int Archer::processArcherPassiveArmorPenalty(int basePenalty) const 
{
    return basePenalty / 2;
}

int Archer::applyArcherPassiveSlowPenalty(int currentDexterity) const 
{
    return (currentDexterity * 3) / 4;
}

int Archer::revertArcherPassiveSlowPenalty(int currentDexterity) const 
{
    return (currentDexterity * 4) / 3;
}

// Habilidade da classe
std::string Archer::getClassAbilityCooldownDescription() const 
{ 
    return "Recarga: 1 turno."; 
}

std::string Archer::getClassAbilityName() const 
{ 
    return "Retirada com pontaria"; 
}

std::string Archer::getClassAbilityDescription() const 
{ 
    return "Se afasta durante um turno, no proximo turno causa 2x damage"; 
}

void Archer::useClassAbility(Combat* combat, Character* userCharacter, std::vector<Character*>& /*listaInimigos*/) 
{
    int remainingTurns = userCharacter->obterRecargaHabilidade(AbilityID::RetreatWithAim);
    if (checkAndReportCooldown(userCharacter, remainingTurns, getClassAbilityName())) return;

    userCharacter->adicionarEfeito(std::make_unique<InviolableEffect>(1));
    userCharacter->definirCooldown(AbilityID::RetreatWithAim, 2);
    
    std::string msg = DialogFunctions::formatarMsgHabilidade("Retirada com pontaria! (Afasta-se)");
    notifyCombatMessage(msg, msg);
}
