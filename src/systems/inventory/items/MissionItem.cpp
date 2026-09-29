#include "MissionItem.h"
#include <functional>
#include <unordered_map>
#include "../ItemFactory.h"

MissionItem::MissionItem(const std::string& name, int price) : Item(price), name(name) {}

std::string MissionItem::getItemName() const { return name; }
EquipmentType MissionItem::getType() const { return EquipmentType::Quest; }

std::vector<std::string> MissionItem::getInspectionDetails(Character* /*personagem*/) const {
    std::vector<std::string> details;
    details.push_back(" > Tipo: Item de Missao");
    if (!inspectionDescription.empty()) {
        for (const auto& desc : inspectionDescription) details.push_back(desc);
    } else {
        details.push_back(" > Lore: Um item misterioso e importante para sua jornada.");
    }
    return details;
}

std::unique_ptr<Item> buildMissionItem(ItemID id) {
    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> constructors = {
        {ItemID::LanguageDevice, []() { 
            auto i = std::make_unique<MissionItem>(ItemFactory::getNameDeID(ItemID::LanguageDevice), 500); 
            i->setInspectionDescription({" > Lore: Um estranho artefato de plastico com teclas.", "   Nao parece pertencer a este mundo, mas emana", "   uma energia peculiar..."});
            return i;
        }}
    };
    auto it = constructors.find(id);
    if (it != constructors.end()) return it->second();
    return nullptr;
}
