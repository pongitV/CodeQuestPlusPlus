#pragma once

#include "../Item.h"
#include <string>
#include <vector>

class MaterialItem : public Item {
private:
    std::string name;
public:
    MaterialItem(const std::string& name, int price = 3);

    std::string getItemName() const override;
    EquipmentType getType() const override;
    std::vector<std::string> getInspectionDetails(Character* character = nullptr) const override;
};

std::unique_ptr<Item> buildMaterialItem(ItemID id);
inline std::unique_ptr<Item> fabricarMaterialItem(ItemID id) { return buildMaterialItem(id); }
