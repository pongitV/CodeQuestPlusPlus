#include "MaterialItem.h"
#include <vector>
#include <functional>
#include <unordered_map>
#include "../ItemFactory.h"

MaterialItem::MaterialItem(const std::string& name, int price) : Item(price), name(name)
{
}

std::string MaterialItem::getItemName() const { return name; }
EquipmentType MaterialItem::getType() const { return EquipmentType::Material; }

std::vector<std::string> MaterialItem::getInspectionDetails(Character* /*personagem*/) const {
    std::vector<std::string> lines;
    lines.push_back(" > Tipo: Material");
    
    if (!inspectionDescription.empty()) {
        for (const auto& desc : inspectionDescription) lines.push_back(" > Descricao: " + desc);
    } else {
        lines.push_back(" > Descricao: Pode ser util para construcoes ou rituais.");
    }
    return lines;
}

std::unique_ptr<Item> buildMaterialItem(ItemID id) {
    auto createMaterial = [](ItemID id, int price, const std::string& desc = "") {
        auto m = std::make_unique<MaterialItem>(ItemFactory::getNameDeID(id), price);
        if (!desc.empty()) m->setInspectionDescription(desc);
        return m;
    };

    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> constructors = {
        {ItemID::AcidSlime, [createMaterial]() { return createMaterial(ItemID::AcidSlime, 5); }},
        {ItemID::GoblinTooth, [createMaterial]() { return createMaterial(ItemID::GoblinTooth, 1, "Pode ser usado na Cabana da Bruxa para encantar armas com Sangramento (Requer 40x)."); }},
        {ItemID::StickyCore, [createMaterial]() { return createMaterial(ItemID::StickyCore, 30, "Pode ser usado na Cabana da Bruxa para encantar armas com Lentidao (Requer 5x)."); }},
        {ItemID::MagicDust, [createMaterial]() { return createMaterial(ItemID::MagicDust, 15, "Pode ser usado na Cabana da Bruxa para encantar armas com Quebra de Resistencia Permanente (Requer 25x)."); }},
        {ItemID::EnchantedWood, [createMaterial]() { return createMaterial(ItemID::EnchantedWood, 3, "Pode ser usada na Cabana da Bruxa para encantar o Arco ou o Violao (Requer 1x)."); }},
        {ItemID::ForestHeart, [createMaterial]() { return createMaterial(ItemID::ForestHeart, 3, "Usado na Cabana da Bruxa para encantar o Cajado ou para desbloquear a passagem do labirinto (Requer 3x)."); }},
        {ItemID::UpgradeStone, [createMaterial]() { return createMaterial(ItemID::UpgradeStone, 3, "Uma pedra extremamente rara. Pode ser usada na Forja de Bjorn para conceder +3 de Defesa (Resistencia) a uma armadura."); }},
        {ItemID::RoyalInvitation, [createMaterial]() { return createMaterial(ItemID::RoyalInvitation, 1, "Permite o acesso livre aos portoes do Kingdom Real."); }}
    };
    auto it = constructors.find(id);
    if (it != constructors.end()) return it->second();
    return nullptr;
}
