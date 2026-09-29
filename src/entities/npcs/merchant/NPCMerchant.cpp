#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <map>

#include "NPCMerchant.h"
#include "../../../ui/screens/menu/ScreenMenu.h"
#include "../../../systems/inventory/Item.h"
#include "../../../systems/inventory/ItemFactory.h"
#include "../../../ui/screens/inventory/ScreenInventory.h"
#include "../../../core/utils/InputControl.h"
#include "../../../core/state/Store.h"
#include "../../../core/utils/DialogFunctions.h"
#include "NPCMerchantLayout.h"
#include "../../../core/utils/Color.h"

namespace {
    std::map<int, StoreProduct> potionsStock = {
        {1, {ItemID::PocaoCura30, 10, -1}}
    };

    std::map<int, StoreProduct> talismansStock = {
        {1, {ItemID::TalismaUrso, 200, 1}},
        {2, {ItemID::TalismaCorvo, 200, 1}},
        {3, {ItemID::TalismaLeopardo, 200, 1}},
        {4, {ItemID::TalismaCoruja, 200, 1}}
    };

    std::map<int, StoreProduct> delicaciesStock = {
        {1, {ItemID::DispositivoLinguagem, 1000, 1}}
    };

    // Aparencia e dialogos
    void processPotionsPurchase(Character* currentPlayer);
    void processTalismansPurchase(Character* currentPlayer);
    void processDelicaciesPurchase(Character* currentPlayer);
    void processItemSales(Character* currentPlayer);

    void franchescoSingleDialogue(const std::string& msg) {
        InputControl::lerSelecaoMenuEmPopup("Franchesco", {msg}, {"OK"}, Color::CYAN, NPCMerchantLayouts::merchantArt);
    }
}

// Informacoes do lugar
std::string NPCMerchant::getPlaceName() const {
    return "MERCADOR AMBULANTE";
}

Color NPCMerchant::getHeaderColor() const {
    return Color::YELLOW;
}

Color NPCMerchant::getArtColor() const {
    return Color::YELLOW;
}

const std::vector<std::string>& NPCMerchant::getASCIIArt() const {
    return NPCMerchantLayouts::merchantArt;
}

// Interacao e menu
void NPCMerchant::interact(Character* player) {
    InputControl::executarLoopMenuPopup(
        [this, player]() { return this->getDialogue(player); },
        [this, player]() { return this->getMenuOptions(player, 120); },
        [this, player](const std::string& op) { this->processOption(player, op, 120); return true; },
        getPlaceName(), getHeaderColor(), getASCIIArt()
    );
}

std::vector<std::string> NPCMerchant::getDialogue(Character* /*jogador*/) {
    return std::vector<std::string>{
        "Bem-vindo! De uma olhada nas",
        "minhas mercadorias."
    };
}

std::vector<std::string> NPCMerchant::getMenuOptions(Character* /*jogador*/, int /*larguraTerminal*/) {
    return {
        "COMPRAR Pocoes",
        "COMPRAR Talismas",
        "COMPRAR Iguarias",
        "VENDER Itens do Inventory",
        "Missoes de Franchesco",
        "VOLTAR"
    };
}

void NPCMerchant::processOption(Character* player, const std::string& option, int /*larguraTerminal*/) {
    if (option == "COMPRAR Pocoes") {
        processPotionsPurchase(player);
    }
    else if (option == "COMPRAR Talismas") {
        processTalismansPurchase(player);
    }
    else if (option == "COMPRAR Iguarias") {
        processDelicaciesPurchase(player);
    }
    else if (option == "VENDER Itens do Inventory") {
        processItemSales(player);
    }
    else if (option == "Missoes de Franchesco") {
        NPCInteraction::processEmptyQuestsMenu(player, "MISSOES DE FRANCHESCO", Color::YELLOW, "Franchesco", "Ah, meu amigo! Nao tenho nenhum pedido especial para voce agora.");
    }
}

namespace {
    // Processamento de opcoes
    void processPotionsPurchase(Character* currentPlayer) {
        Store::processPurchase(currentPlayer, "LOJA - POCOES", Color::YELLOW, potionsStock, 
            [](const std::string& msg) { franchescoSingleDialogue(msg); }, NPCInteraction::getItemStatusFormatter, NPCMerchantLayouts::merchantArt);
    }

    void processTalismansPurchase(Character* currentPlayer) {
        Store::processPurchase(currentPlayer, "LOJA - TALISMAS", Color::YELLOW, talismansStock, 
            [](const std::string& msg) { franchescoSingleDialogue(msg); }, NPCInteraction::getItemStatusFormatter, NPCMerchantLayouts::merchantArt);
    }

    void processDelicaciesPurchase(Character* currentPlayer) {
        Store::processPurchase(currentPlayer, "LOJA - IGUARIAS", Color::YELLOW, delicaciesStock, 
            [](const std::string& msg) { franchescoSingleDialogue(msg); }, NPCInteraction::getItemStatusFormatter, NPCMerchantLayouts::merchantArt);
    }

    void processItemSales(Character* currentPlayer) {
        do {
            std::vector<std::pair<std::string, std::vector<Item*>>> itemGroups;
            std::map<std::string, int> indexMap;
            
            for (auto* item : currentPlayer->getInventory()->getAllItems()) {
                if (item->getType() != EquipmentType::Quest) {
                    std::string itemName = item->getItemName();
                    bool equipped = currentPlayer->isItemEquipped(item);
                    std::string key = itemName;
                    if (equipped) {
                        key += " [Equipado]";
                    }
                    
                    if (indexMap.find(key) == indexMap.end()) {
                        indexMap[key] = itemGroups.size();
                        itemGroups.push_back({key, {item}});
                    } else {
                        itemGroups[indexMap[key]].second.push_back(item);
                    }
                }
            }

            std::vector<std::vector<Item*>> validItems;
            std::vector<std::string> itemOptions;
            
            for (const auto& group : itemGroups) {
                int count = group.second.size();
                Item* exampleItem = group.second.front();
                int sellPrice = exampleItem->getSellPrice();
                
                std::string optionText = group.first + " (" + std::to_string(sellPrice) + "G)";
                if (count > 1) {
                    optionText += " x" + std::to_string(count);
                }
                
                validItems.push_back(group.second);
                itemOptions.push_back(optionText);
            }

            if (itemOptions.empty()) { franchescoSingleDialogue("Voce nao tem nada que me interesse!"); break; }
            itemOptions.push_back("VOLTAR");
            
            int choice = InputControl::lerSelecaoMenuEmPopup("VENDER ITENS", {"Seu Ouro: " + std::to_string(currentPlayer->getInventory()->getGold()) + "G", "Escolha um item para vender:"}, itemOptions, Color::YELLOW, NPCMerchantLayouts::merchantArt);
            if (choice == -1 || choice == static_cast<int>(itemOptions.size()) - 1) break;
            
            std::vector<Item*> chosenItems = validItems[choice];
            Item* itemToSell = chosenItems.front();

            if (currentPlayer->isItemEquipped(itemToSell)) {
                franchescoSingleDialogue("Nao e possivel vender itens que estao equipados!");
                continue;
            }
            
            int amountToSell = 1;
            if (chosenItems.size() > 1) {
                std::vector<std::string> qtyOptions = {
                    "Vender 1 unidade",
                    "Vender Todos (" + std::to_string(chosenItems.size()) + " unidades)",
                    "Digitar amount...",
                    "Cancelar"
                };
                
                int qtyChoice = InputControl::lerSelecaoMenuEmPopup(
                    "QUANTIDADE: " + itemToSell->getItemName(),
                    {"Voce possui " + std::to_string(chosenItems.size()) + " unidades deste item."},
                    qtyOptions, 
                    Color::YELLOW, 
                    NPCMerchantLayouts::merchantArt
                );
                
                if (qtyChoice == 0) {
                    amountToSell = 1;
                } else if (qtyChoice == 1) {
                    amountToSell = chosenItems.size();
                } else if (qtyChoice == 2) {
                    amountToSell = 0; // Nao suportado mais
                } else {
                    continue; // Cancelar
                }
            }
            
            if (amountToSell == 0) continue;

            std::string itemName = itemToSell->getItemName();
            int unitSellPrice = itemToSell->getSellPrice();
            int totalGain = unitSellPrice * amountToSell;
            
            currentPlayer->getInventory()->addGold(totalGain);
            for (int i = 0; i < amountToSell; ++i) {
                currentPlayer->getInventory()->removeItem(chosenItems[i]);
            }
            
            if (amountToSell > 1) {
                franchescoSingleDialogue("Voce vendeu " + std::to_string(amountToSell) + "x " + itemName + " por " + std::to_string(totalGain) + "G!");
            } else {
                franchescoSingleDialogue("Voce vendeu " + itemName + " por " + std::to_string(totalGain) + "G!");
            }
        } while (true);
    }
}
