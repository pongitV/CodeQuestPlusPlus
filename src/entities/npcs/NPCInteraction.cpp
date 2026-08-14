#include "NPCInteraction.h"
#include <iostream>
#include "../../core/utils/InputControl.h"
#include "../../ui/screens/inventory/ScreenInventory.h"
#include "../../systems/inventory/ItemFactory.h"
#include "../../core/utils/DialogFunctions.h"
#include "../../ui/screens/ScreenBase.h"
#include "../../core/utils/Color.h"

// --- INTERACAO PRINCIPAL ---
void NPCInteraction::interact(Character* currentPlayer) {
    InputControl::executarLoopMenuPopup(
        [this, currentPlayer]() { return this->getDialogue(currentPlayer); },
        [this, currentPlayer]() { return this->getMenuOptions(currentPlayer, 120); },
        [this, currentPlayer](const std::string& op) { this->processOption(currentPlayer, op, 120); return true; },
        getPlaceName(), getHeaderColor(), getASCIIArt()
    );
}

void NPCInteraction::processEmptyQuestsMenu(Character* /*currentPlayer*/, const std::string& menuTitle, Color headerColor, const std::string& /*npcName*/, const std::string& /*emptyDialogue*/) {
    std::string questOption;
    do {
        std::vector<std::string> quests = {
            "(Nenhuma missao disponivel)",
            "VOLTAR"
        };
        
        int id = InputControl::lerSelecaoMenuEmPopup(menuTitle, {"Escolha uma missao:"}, quests, headerColor);
        if (id == -1) break;
        questOption = quests[id];

        if (questOption == "(Nenhuma missao disponivel)") {
        }
    } while (questOption != "VOLTAR");
}

bool NPCInteraction::verifyMaterialInInventory(Character* currentPlayer, const std::string& materialName, int requiredAmount, const std::string& npcName, Color npcColor, const std::string& customMessage) {
    int currentAmount = currentPlayer->getInventory()->getItemCount(materialName);
    if (currentAmount < requiredAmount) {
        std::string text = customMessage.empty()
            ? "Voce precisa de " + std::to_string(requiredAmount) + "x " + materialName + " para isso!"
            : customMessage;
        InputControl::lerSelecaoMenuEmPopup(npcName, {text}, {"OK"}, npcColor);
        return false;
    }
    return true;
}

Item* NPCInteraction::readItemFromInventory(Character* currentPlayer, const std::string& /*dialogueMessage*/, const std::string& /*npcName*/, Color /*npcColor*/, std::string& exitCode, bool displayPrices) {
    Item* selectedItem = nullptr;

    TelaBase::executarLoop(
        [](bool animar) { TelaInventario::displayCabecalhoInventario(animar); },
        [&]() {
        },
        [currentPlayer, displayPrices]() {
            std::vector<std::string> options;
            options.push_back("Arsenal de Equipamentos");
            options.push_back("Itens Consumiveis");
            options.push_back("Estoque e Materiais");
            options.push_back("Itens de Missao");
            options.push_back("VOLTAR");
            return options;
        },
        [&](int categoryChoice) {
            if (categoryChoice < 0 || categoryChoice == 4) {
                exitCode = "0";
                return false;
            }
            
            auto categoryItems = TelaInventario::obterListaCategoria(currentPlayer, categoryChoice, displayPrices);
            
            TelaBase::executarLoop(
                [](bool animar) { TelaInventario::displayCabecalhoInventario(animar); },
                [&]() {},
                [&categoryItems]() {
                    std::vector<std::string> options;
                    for (auto& pair : categoryItems) options.push_back(pair.first);
                    options.push_back("VOLTAR");
                    return options;
                },
                [&](int itemChoice) {
                    if (itemChoice < 0 || itemChoice >= static_cast<int>(categoryItems.size())) {
                        return false;
                    }
                    selectedItem = categoryItems[itemChoice].second;
                    exitCode = "selecionado";
                    return false;
                }
            );

            if (selectedItem) return false;
            return true;
        }
    );
    
    return selectedItem;
}

void NPCInteraction::displaySuccessScreen(const std::string& headerTitle, Color headerColor, const std::string& /*equation*/, const std::vector<std::string>& asciiArt, const std::string& /*npcName*/, const std::string& npcDialogue) {
    InputControl::lerSelecaoMenuEmPopup(headerTitle, {npcDialogue}, {"OK"}, headerColor, asciiArt);
}

std::string NPCInteraction::getItemStatusFormatter(ItemID id) {
    std::unique_ptr<Item> tempItem = ItemFactory::createItem(id);
    return tempItem ? tempItem->getStatusInfo() : "";
}

bool NPCInteraction::verifyItemNotEquipped(Character* currentPlayer, Item* evaluatedItem, const std::string& npcName, Color npcColor, const std::string& errorMessage) {
    if (currentPlayer->isItemEquipped(evaluatedItem)) {
        InputControl::lerSelecaoMenuEmPopup(npcName, {errorMessage}, {"OK"}, npcColor);
        return false;
    }
    return true;
}
