#pragma once

#include "../Item.h"
#include <string>

class MissionItem : public Item {
private:
    std::string name;
public:
    MissionItem(const std::string& name, int price = 500);
    std::string getItemName() const override;
    EquipmentType getType() const override;
    std::vector<std::string> getInspectionDetails(Character* character = nullptr) const override;
};

std::unique_ptr<Item> buildMissionItem(ItemID id);
inline std::unique_ptr<Item> fabricarMissionItem(ItemID id) { return buildMissionItem(id); }
