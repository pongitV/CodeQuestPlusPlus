#pragma once

#include "../Item.h"
#include <string>

class ConsumableItem : public Item
{
private:
    std::string name;

public:
    ConsumableItem(const std::string& name, int price = 3);

    std::string getItemName() const override;
    EquipmentType getType() const override;
    std::vector<std::string> getInspectionDetails(Character* character = nullptr) const override;
};

using ItemConsumivel = ConsumableItem;

std::unique_ptr<Item> buildConsumableItem(ItemID id);
inline std::unique_ptr<Item> fabricarItemConsumivel(ItemID id) { return buildConsumableItem(id); }
