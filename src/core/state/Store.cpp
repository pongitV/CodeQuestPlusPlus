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
    for (auto& par : currentStock) {
        sortedItems.emplace_back(par.first, &par.second);
    }

    std::sort(sortedItems.begin(), sortedItems.end(), [&itemNameFormatter](const auto& a, const auto& b) {
        std::string nomeA = ItemFactory::getNameDeID(a.second->itemId);
        if (itemNameFormatter) nomeA += itemNameFormatter(a.second->itemId);
        std::string nomeB = ItemFactory::getNameDeID(b.second->itemId);
        if (itemNameFormatter) nomeB += itemNameFormatter(b.second->itemId);
        return nomeA < nomeB;
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

        for (const auto& par : sortedItems) {
            auto* produto = par.second;
            std::string itemLine = ItemFactory::getNameDeID(produto->itemId);
            if (itemNameFormatter) itemLine += itemNameFormatter(produto->itemId);
            itemLine += " - " + std::to_string(produto->price) + "G";
            if (produto->amount == 0) itemLine += " (Out of Stock)";
            else if (produto->amount != -1) itemLine += " (Stock: " + std::to_string(produto->amount) + ")";
            
            strBuf.Append(itemLine);
        }
        strBuf.Append("BACK");
        
        std::vector<std::string> opcoes = strBuf.ToVector();
        
        int escolha = InputControl::lerSelecaoMenuEmPopup(shopTitle, text, opcoes, shopColor, asciiArt, animateEntry);
        animateEntry = false;
        
        if (escolha == -1 || escolha == static_cast<int>(sortedItems.size())) {
            break;
        }
            
        auto* produto = sortedItems[escolha].second;

            if (produto->amount == 0) {
                displayNPCDialog("This item is out of stock!");
            } else {
                int maxBuyer = currentPlayer->obterInventario()->obterOuro() / produto->price;
                if (maxBuyer == 0) {
                    displayNPCDialog("You don't have enough gold for this!");
                } else {
                    int maxPossible = (produto->amount == -1) ? maxBuyer : std::min(maxBuyer, produto->amount);
                int qtdComprar = 1;
                
                if (maxPossible > 1) {
                    std::string nomeDoItem = ItemFactory::getNameDeID(produto->itemId);
                    if (itemNameFormatter) nomeDoItem += itemNameFormatter(produto->itemId);
                    
                    std::vector<std::string> opcoesQtd = {
                        "Buy 1 unit",
                        "Buy Max (" + std::to_string(maxPossible) + " units)",
                        "Enter amount...",
                        "Cancel"
                    };
                    
                    int escolhaQtd = InputControl::lerSelecaoMenuEmPopup(
                        "QUANTITY: " + nomeDoItem,
                        {"Unit Price: " + std::to_string(produto->price) + "G", "You can buy up to " + std::to_string(maxPossible) + " units."},
                        opcoesQtd, 
                        shopColor,
                        asciiArt,
                        false // Never animate shop submenus
                    );
                    
                    if (escolhaQtd == 0) {
                        qtdComprar = 1;
                    } else if (escolhaQtd == 1) {
                        qtdComprar = maxPossible;
                    } else if (escolhaQtd == 2) {
                        qtdComprar = InputControl::lerInteiroComLimites("Quantity", 1, maxPossible, true, "");
                    } else {
                        qtdComprar = 0; // Cancel
                    }
                }

                    if (qtdComprar > 0) {
                        currentPlayer->obterInventario()->adicionarOuro(-(produto->price * qtdComprar));
                        if (produto->amount != -1) produto->amount -= qtdComprar;
                        std::string nomeNovo = ItemFactory::getNameDeID(produto->itemId);
                        for (int i = 0; i < qtdComprar; ++i) currentPlayer->obterInventario()->adicionarItem(ItemFactory::criarItem(produto->itemId));
                        Diary::instancia().registrarItem(nomeNovo);
                        displayNPCDialog(std::to_string(qtdComprar) + "x " + nomeNovo + " bought!");
                    }
                }
            }
    }
}
