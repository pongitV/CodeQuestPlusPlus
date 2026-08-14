#include <algorithm>
#include <unordered_map>

#include "Inventory.h"
#include "Item.h"
#include "./items/ConsumableItem.h"
#include "../../core/state/GameMenu.h"

Inventory::Inventory() : goldAmount(0) {}

bool Inventory::isEmpty() const { return itemList.empty(); }

int Inventory::getGold() const { return goldAmount; }

int Inventory::countItem(const std::string& itemName) const 
{
    auto it = itemCountMap_.find(itemName);
    return it != itemCountMap_.end() ? it->second : 0;
}

void Inventory::addGold(int additionalAmount) 
{ 
    goldAmount = std::max(0, goldAmount + additionalAmount); 
}

std::vector<ItemIndex> Inventory::getSortedIndices() const {
    std::vector<ItemIndex> indices;
    indices.reserve(itemList.size());
    for (size_t i = 0; i < itemList.size(); ++i) {
        if (itemList[i]) {
            indices.push_back({i, itemList[i]->getItemName(), EquipmentType::None, false});
        }
    }
    std::sort(indices.begin(), indices.end(), [](const ItemIndex& a, const ItemIndex& b) {
        return a.name < b.name;
    });
    return indices;
}

void Inventory::addItem(std::unique_ptr<Item> newItem) 
{ 
    if (newItem) {
        itemCountMap_[newItem->getItemName()]++;
        countCache_.invalidate();
        itemList.push_back(std::move(newItem));
    }
}

namespace {
    void decrementCountAndErase(std::vector<std::unique_ptr<Item>>& list, std::unordered_map<std::string, int>& countMap, std::vector<std::unique_ptr<Item>>::iterator it) {
        std::string name = (*it)->getItemName();
        auto mapIt = countMap.find(name);
        if (mapIt != countMap.end()) {
            if (--mapIt->second <= 0) {
                countMap.erase(mapIt);
            }
        }
        list.erase(it);
    }
}

void Inventory::removeItem(const std::string& itemName) 
{
    auto it = std::find_if(itemList.begin(), itemList.end(), [&](const std::unique_ptr<Item>& item) 
    {
        return item->getItemName() == itemName;
    });
    
    if (it != itemList.end()) 
    {
        decrementCountAndErase(itemList, itemCountMap_, it);
    }
}

void Inventory::removeItem(Item* exactItem) 
{
    if (!exactItem) return;
    auto it = std::find_if(itemList.begin(), itemList.end(), [&](const std::unique_ptr<Item>& item) 
    {
        return item.get() == exactItem;
    });
    
    if (it != itemList.end()) {
        decrementCountAndErase(itemList, itemCountMap_, it);
    }
}

Item* Inventory::findItemByCode(const std::string& enteredCode, Item* equippedWeapon, Item* equippedShield, Item* equippedArmor)
{
    if (enteredCode.length() < 2) return nullptr;

    char categoryLetter = std::toupper(enteredCode.back());
    std::string numericPart = enteredCode.substr(0, enteredCode.length() - 1);
    
    if (!std::all_of(numericPart.begin(), numericPart.end(), ::isdigit)) return nullptr;
    
    int itemIndex = std::stoi(numericPart);
    if (itemIndex <= 0) return nullptr;

    if (categoryLetter == 'E')
    {
        if (itemIndex == 1) return equippedWeapon;
        if (itemIndex == 2) return equippedShield;
        if (itemIndex == 3) return equippedArmor;
        return nullptr;
    }

    auto searchByGroupedType = [&](auto condition) -> Item* {
        std::unordered_map<std::string, size_t> indexCache;
        std::vector<std::string> displayedItemNames;

        for (size_t i = 0; i < itemList.size(); ++i) {
            Item* currentItem = itemList[i].get();
            if (condition(currentItem)) {
                if (indexCache.find(currentItem->getItemName()) == indexCache.end()) {
                    indexCache[currentItem->getItemName()] = i;
                    displayedItemNames.push_back(currentItem->getItemName());
                }
            }
        }
        
        if (itemIndex > 0 && itemIndex <= static_cast<int>(displayedItemNames.size())) {
            size_t originalInventoryIndex = indexCache[displayedItemNames[itemIndex - 1]];
            return itemList[originalInventoryIndex].get();
        }
        return nullptr;
    };

    switch (categoryLetter) {
        case 'A':
            return searchByGroupedType([&](Item* evaluatedItem) { 
                return evaluatedItem->isEquippable() && evaluatedItem != equippedWeapon && evaluatedItem != equippedShield && evaluatedItem != equippedArmor; 
            });
        case 'C': return searchByGroupedType([](Item* evaluatedItem) { return evaluatedItem->getType() == EquipmentType::Consumable; });
        case 'S': return searchByGroupedType([](Item* evaluatedItem) { return evaluatedItem->getType() == EquipmentType::Material; });
        case 'M': return searchByGroupedType([](Item* evaluatedItem) { return evaluatedItem->getType() == EquipmentType::Quest; });
        default:  return nullptr;
    }
}
