#include "Store.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "../../systems/inventory/ItemFactory.h"
#include "../utils/InputControl.h"
#include "../utils/StringBuffer.h"
#include "../../systems/progress/Diary.h"
#include "../../ui/screens/ScreenBase.h"
#include "../../core/utils/Color.h"

void Store::processPurchase(Character* currentPlayer, const std::string& shopTitle, Color shopColor, 
                            std::map<int, StoreProduct>& currentStock, 
                            const std::function<void(const std::string&)>& displayNPCDialog, 
                            const std::function<std::string(ItemID)>& itemNameFormatter,
                            const std::vector<std::string>& asciiArt) {
    std::vector<std::pair<int, StoreProduct*>> sortedItems;
    sortedItems.reserve(currentStock.size());
    for (auto& pair : currentStock) {
        sortedItems.emplace_back(pair.first, &pair.second);
    }

    std::sort(sortedItems.begin(), sortedItems.end(), [&itemNameFormatter](const auto& a, const auto& b) {
        std::string nameA = ItemFactory::getNameDeID(a.second->itemId);
        if (itemNameFormatter) nameA += itemNameFormatter(a.second->itemId);
        std::string nameB = ItemFactory::getNameDeID(b.second->itemId);
        if (itemNameFormatter) nameB += itemNameFormatter(b.second->itemId);
        return nameA < nameB;
    });

    bool animateEntry = true;
    StringBuffer strBuf(sortedItems.size() + 2);

    while (true) {
        strBuf.Clear();
        strBuf.Append("Your Gold: ");
        strBuf.Append(currentPlayer->obterInventario()->obterOuro());
        strBuf.Append("G");
        
        std::vector<std::string> text = strBuf.ToVector();
        strBuf.Clear();

        for (const auto& pair : sortedItems) {
            auto* product = pair.second;
            std::string itemLine = ItemFactory::getNameDeID(product->itemId);
            if (itemNameFormatter) itemLine += itemNameFormatter(product->itemId);
            itemLine += " - " + std::to_string(product->price) + "G";
            if (product->amount == 0) itemLine += " (Out of Stock)";
            else if (product->amount != -1) itemLine += " (Stock: " + std::to_string(product->amount) + ")";
            
            strBuf.Append(itemLine);
        }
        strBuf.Append("BACK");
        
        std::vector<std::string> options = strBuf.ToVector();
        
        int choice = InputControl::readMenuSelectionInPopup(shopTitle, text, options, shopColor, asciiArt, animateEntry);
        animateEntry = false;
        
        if (choice == -1 || choice == static_cast<int>(sortedItems.size())) {
            break;
        }
            
        auto* product = sortedItems[choice].second;

        if (product->amount == 0) {
            displayNPCDialog("This item is out of stock!");
        } else {
            int maxBuyer = currentPlayer->obterInventario()->obterOuro() / product->price;
            if (maxBuyer == 0) {
                displayNPCDialog("You don't have enough gold for this!");
            } else {
                int maxPossible = (product->amount == -1) ? maxBuyer : std::min(maxBuyer, product->amount);
                int buyAmount = 1;
                
                if (maxPossible > 1) {
                    std::string itemName = ItemFactory::getNameDeID(product->itemId);
                    if (itemNameFormatter) itemName += itemNameFormatter(product->itemId);
                    
                    std::vector<std::string> optionsQty = {
                        "Buy 1 unit",
                        "Buy Max (" + std::to_string(maxPossible) + " units)",
                        "Enter amount...",
                        "Cancel"
                    };
                    
                    int choiceQty = InputControl::readMenuSelectionInPopup(
                        "QUANTITY: " + itemName,
                        {"Unit Price: " + std::to_string(product->price) + "G", "You can buy up to " + std::to_string(maxPossible) + " units."},
                        optionsQty, 
                        shopColor,
                        asciiArt,
                        false // Nunca anima submenus da loja
                    );
                    
                    if (choiceQty == 0) {
                        buyAmount = 1;
                    } else if (choiceQty == 1) {
                        buyAmount = maxPossible;
                    } else if (choiceQty == 2) {
                        buyAmount = InputControl::readIntegerWithBounds("Quantity", 1, maxPossible, true, "");
                    } else {
                        buyAmount = 0; // Cancel
                    }
                }

                if (buyAmount > 0) {
                    currentPlayer->obterInventario()->adicionarOuro(-(product->price * buyAmount));
                    if (product->amount != -1) product->amount -= buyAmount;
                    std::string newName = ItemFactory::getNameDeID(product->itemId);
                    for (int i = 0; i < buyAmount; ++i) currentPlayer->obterInventario()->adicionarItem(ItemFactory::criarItem(product->itemId));
                    Diary::instance().registerItem(newName);
                    displayNPCDialog(std::to_string(buyAmount) + "x " + newName + " bought!");
                }
            }
        }
    }
}
