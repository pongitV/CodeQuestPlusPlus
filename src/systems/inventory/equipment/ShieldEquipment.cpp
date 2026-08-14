#include "ShieldEquipment.h"
#include <memory>
#include "../../../entities/character/Character.h"
#include <vector>
#include <functional>
#include <unordered_map>
#include "../../../ui/screens/combat/ScreenCombat.h"
#include "../ItemFactory.h"

ShieldEquipment::ShieldEquipment(const std::string& name, int fixedReduction, int durability, int reqResistance, int reqSecondary, AttributeType secondaryType, int price)
    : BaseEquipment(price), name(name), fixedReduction(fixedReduction), durability(durability), maxDurability(durability), reqResistance(reqResistance), reqSecondary(reqSecondary), secondaryType(secondaryType)
{
}

std::string ShieldEquipment::getItemName() const { return name; }
EquipmentType ShieldEquipment::getType() const { return EquipmentType::Shield; }

int ShieldEquipment::getMaxDurability() const { return maxDurability; }
void ShieldEquipment::setDurability(int newDurability) { durability = newDurability; }
int ShieldEquipment::getShieldCurrentDurability() const { return durability; }
int ShieldEquipment::getShieldFixedDamageReduction() const { return (durability > 0) ? fixedReduction : 0; }

int ShieldEquipment::getReqResistance() const { return reqResistance; }
int ShieldEquipment::getReqSecondary() const { return reqSecondary; }
AttributeType ShieldEquipment::getSecondaryType() const { return secondaryType; }

void ShieldEquipment::reduceDurability(int qty) { 
    if (durability <= 0) return; // Ja estava quebrado
    
    durability -= qty; 
    if (durability <= 0) {
        durability = 0;
        TelaCombate::adicionarMensagemFixa(TelaCombate::margemCombate() + ">> O escudo [" + name + "] quebrou e perdeu seu poder de bloqueio!\n");
    }
}
void ShieldEquipment::increaseDurability(int qty) { durability += qty; }

bool Item::checkShieldRequirements(Character* character, int reqResistance, int secVal, int reqSecondary) {
    if (!character) return false;
    return character->getResistance() >= reqResistance && secVal >= reqSecondary;
}

bool ShieldEquipment::checkSpecificRequirements(Character* character) const {
    int secVal = 0;
    switch(secondaryType) {
        case AttributeType::Strength: secVal = character->getStrength(); break;
        case AttributeType::Dexterity: secVal = character->getDexterity(); break;
        case AttributeType::Intelligence: secVal = character->getIntelligence(); break;
        case AttributeType::Wisdom: secVal = character->getWisdom(); break;
        default: secVal = 9999;
    }
    return Item::checkShieldRequirements(character, reqResistance, secVal, reqSecondary);
}

bool ShieldEquipment::canBeEquippedBy(Character* character) const {
    return BaseEquipment::canBeEquippedBy(character);
}

std::vector<std::string> ShieldEquipment::getInspectionDetails(Character* character) const {
    std::vector<std::string> lines;
    lines.push_back(" > Tipo: Shield");

    std::string bloqueioStr = std::to_string(fixedReduction) + " (Damage bloqueado na acao 'Defender')";
    if (character) {
        int defTotal = fixedReduction + character->getResistance();
        bloqueioStr += " -> C/ Seus Attributes: " + std::to_string(defTotal);
    }
    lines.push_back(" > Poder de Bloqueio: " + bloqueioStr);
    lines.push_back(" > Durabilidade Maxima: " + std::to_string(durability) + " usos");
    lines.push_back(" > Requisitos:");
    bool hasReq = false;
    if (reqResistance > 0) { lines.push_back("   - Resistencia Base: " + std::to_string(reqResistance)); hasReq = true; }
    if (reqSecondary > 0) {
        std::string atrSec = "";
        if (secondaryType == AttributeType::Strength) atrSec = "Forca";
        else if (secondaryType == AttributeType::Dexterity) atrSec = "Destreza";
        else if (secondaryType == AttributeType::Intelligence) atrSec = "Inteligencia";
        else if (secondaryType == AttributeType::Wisdom) atrSec = "Sabedoria";
        lines.push_back("   - Atributo Secundario (" + atrSec + "): " + std::to_string(reqSecondary));
        hasReq = true;
    }
    if (!hasReq) lines.push_back("   - Nenhum requisito.");
    return lines;
}

std::string ShieldEquipment::getStatusInfo() const {
    std::string info = " (Def: " + std::to_string(fixedReduction) + " | Dur: " + std::to_string(durability) + "/" + std::to_string(maxDurability);
    std::string reqs = "";
    if (reqResistance > 0 || reqSecondary > 0) {
        reqs += " | Req: ";
        if (reqResistance > 0) reqs += std::to_string(reqResistance) + " Res ";
        if (reqSecondary > 0) {
            reqs += std::to_string(reqSecondary) + " ";
            if (secondaryType == AttributeType::Strength) reqs += "For ";
            else if (secondaryType == AttributeType::Dexterity) reqs += "Des ";
            else if (secondaryType == AttributeType::Intelligence) reqs += "Int ";
            else if (secondaryType == AttributeType::Wisdom) reqs += "Sab ";
        }
    }
    
    std::string tag = "";
    if (durability <= 0) {
        tag = " [QUEBRADO]";
    } else if (durability < maxDurability) {
        tag = " [D]";
    }
    return info + reqs + ")" + tag;
}

std::unique_ptr<Item> ShieldEquipment::generateUpgradedCopy() const {
    auto newShield = std::make_unique<ShieldEquipment>(name + "+", static_cast<int>(fixedReduction * 1.5), static_cast<int>(maxDurability * 1.5), reqResistance, reqSecondary, secondaryType, sellPrice * 2);
    for (Property prop : properties) newShield->addProperty(prop);
    newShield->addProperty(Property::Upgraded);
    return newShield;
}

std::unique_ptr<Item> buildShieldEquipment(ItemID id) {
    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> constructors = {
        {ItemID::MetalShield, []() { return std::make_unique<ShieldEquipment>(ItemFactory::getNameDeID(ItemID::MetalShield), 15, 5, 0, 0, AttributeType::Strength, 9); }},
        {ItemID::MagicBarrier, []() { return std::make_unique<ShieldEquipment>(ItemFactory::getNameDeID(ItemID::MagicBarrier), 50, 2, 0, 0, AttributeType::Intelligence, 3); }},
        {ItemID::MagicCape, []() { return std::make_unique<ShieldEquipment>(ItemFactory::getNameDeID(ItemID::MagicCape), 6, 10, 0, 0, AttributeType::Wisdom, 9); }},
        {ItemID::SilverBracers, []() { return std::make_unique<ShieldEquipment>(ItemFactory::getNameDeID(ItemID::SilverBracers), 5, 3, 0, 0, AttributeType::Dexterity, 3); }}
    };
    auto it = constructors.find(id);
    if (it != constructors.end()) return it->second();
    return nullptr;
}
