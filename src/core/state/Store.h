#pragma once
#include <map>
#include <string>
#include <functional>
#include <vector>
#include "../../entities/character/Character.h"
#include "../../systems/inventory/Item.h"
#include "../../core/utils/Color.h"
struct StoreProduct {
    ItemID itemId;
    int price;
    int amount; // -1 para infinito
};

class Store {
public:
    static void processPurchase(Character* currentPlayer, const std::string& shopTitle, Color shopColor, 
                                std::map<int, StoreProduct>& currentStock, 
                                const std::function<void(const std::string&)>& displayNPCDialog, 
                                const std::function<std::string(ItemID)>& itemNameFormatter = nullptr,
                                const std::vector<std::string>& asciiArt = {});
};
