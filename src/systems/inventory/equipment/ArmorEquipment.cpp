#include "ArmorEquipment.h"
#include "../../../entities/character/Character.h"
#include <vector>
#include <functional>
#include <unordered_map>
#include "../ItemFactory.h"

ArmorEquipment::ArmorEquipment(const std::string& name, int fixedReduction, int reqResistance, int reqConstitution, int price) 
    : BaseEquipment(price), name(name), fixedReduction(fixedReduction), reqResistance(reqResistance), reqConstitution(reqConstitution), dexterityPenalty(fixedReduction / 3)
{
}

std::string ArmorEquipment::getItemName() const { return name; }
EquipmentType ArmorEquipment::getType() const { return EquipmentType::Armor; }

int ArmorEquipment::getFixedReduction() const { return fixedReduction; }
int ArmorEquipment::getReqResistance() const { return reqResistance; }
int ArmorEquipment::getReqConstitution() const { return reqConstitution; }

bool Item::checkArmorRequirements(Character* character, int reqResistance, int reqConstitution) {
    if (!character) return false;
    return character->getResistance() >= reqResistance &&
           character->getConstitution() >= reqConstitution;
}

bool ArmorEquipment::checkSpecificRequirements(Character* character) const {
    return Item::checkArmorRequirements(character, reqResistance, reqConstitution);
}

bool ArmorEquipment::canBeEquippedBy(Character* character) const {
    return BaseEquipment::canBeEquippedBy(character);
}

std::vector<std::string> ArmorEquipment::getInspectionDetails(Character* character) const {
    std::vector<std::string> lines;
    lines.push_back(" > Tipo: Armor");
    
    std::string defFixaStr = std::to_string(fixedReduction) + " (Reduz damage recebido permanentemente)";
    if (character) {
        int defTotal = fixedReduction + character->getResistance();
        defFixaStr += " -> C/ Seus Attributes: " + std::to_string(defTotal);
    }
    lines.push_back(" > Defesa Fixa: " + defFixaStr);
    lines.push_back(" > Requisitos:");
    bool hasReq = false;
    if (reqResistance > 0) { lines.push_back("   - Resistencia: " + std::to_string(reqResistance)); hasReq = true; }
    if (reqConstitution > 0) { lines.push_back("   - Constituicao: " + std::to_string(reqConstitution)); hasReq = true; }
    if (!hasReq) lines.push_back("   - Nenhum requisito.");
    
    if (dexterityPenalty > 0) {
        lines.push_back(" > Penalidade: -" + std::to_string(dexterityPenalty) + " Destreza");
    } else {
        lines.push_back(" > Penalidade: Nenhuma");
    }
    
    if (hasProperty(Property::AdaptationArmor)) {
        lines.push_back(" > Efeitos Ocultos: A Roda gira a cada turno regenerando 5% do HP e adapta a");
        lines.push_back("                    sua defesa ao enemy e seu ataque a sua arma (+2 status)!");
    }
    return lines;
}

std::string ArmorEquipment::getStatusInfo() const {
    std::string info = " (Def: " + std::to_string(fixedReduction);
    if (dexterityPenalty > 0) {
        info += " | -" + std::to_string(dexterityPenalty) + " Dest";
    }
    
    std::string reqs = "";
    if (reqResistance > 0 || reqConstitution > 0) {
        reqs += " | Req: ";
        if (reqResistance > 0) reqs += std::to_string(reqResistance) + " Res ";
        if (reqConstitution > 0) reqs += std::to_string(reqConstitution) + " Con ";
    }

    return info + reqs + ")";
}

std::unique_ptr<Item> ArmorEquipment::generateUpgradedCopy() const {
    auto newArmor = std::make_unique<ArmorEquipment>(name + "+", static_cast<int>(fixedReduction * 1.5), reqResistance, reqConstitution, sellPrice * 2);
    for (Property prop : properties) newArmor->addProperty(prop);
    newArmor->addProperty(Property::Upgraded);
    return newArmor;
}

std::unique_ptr<Item> buildArmorEquipment(ItemID id) {
    auto createArmor = [](ItemID id, int def, int rRes, int rCon, int price) {
        return std::make_unique<ArmorEquipment>(ItemFactory::getNameDeID(id), def, rRes, rCon, price);
    };

    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> constructors = {
        {ItemID::ChainArmor, [createArmor]() { return createArmor(ItemID::ChainArmor, 7, 0, 0, 3); }},
        {ItemID::LeatherArmor, [createArmor]() { return createArmor(ItemID::LeatherArmor, 5, 0, 0, 3); }},
        {ItemID::Tunic, [createArmor]() { return createArmor(ItemID::Tunic, 2, 0, 0, 3); }},
        {ItemID::NobleOutfit, [createArmor]() { return createArmor(ItemID::NobleOutfit, 4, 0, 0, 3); }},
        {ItemID::RagsArmor, [createArmor]() { return createArmor(ItemID::RagsArmor, 3, 0, 0, 3); }},
        {ItemID::KnightArmor, [createArmor]() { return createArmor(ItemID::KnightArmor, 12, 0, 0, 0); }},
        {ItemID::RitualistClothes, [createArmor]() { return createArmor(ItemID::RitualistClothes, 3, 0, 0, 15); }},
        {ItemID::ChestArmor, [createArmor]() { 
            auto armor = createArmor(ItemID::ChestArmor, 20, 0, 0, 150); 
            armor->setDexterityPenalty(10);
            return armor; 
        }},
        {ItemID::AdaptationWheel, []() { 
            std::string name = "Roda da Adaptacao";
            auto armor = std::make_unique<ArmorEquipment>(name, 40, 0, 0, 5000); 
            armor->setDexterityPenalty(0);
            armor->addProperty(Property::AdaptationArmor);
            return armor; 
        }}
    };
    auto it = constructors.find(id);
    if (it != constructors.end()) return it->second();
    return nullptr;
}
