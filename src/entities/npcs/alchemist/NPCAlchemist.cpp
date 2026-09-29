#include "NPCAlchemist.h"
#include "NPCAlchemistLayout.h"
#include "../../../core/state/GameMenu.h"
#include "../../../core/utils/InputControl.h"
#include "../../../systems/inventory/ItemFactory.h"
#include <iostream>
#include "../../../core/utils/Color.h"

void NPCAlchemist::interact(Character* player) {
    InputControl::executarLoopMenuPopup(
        [this, player]() { return this->getDialogue(player); },
        [this, player]() { return this->getMenuOptions(player, 120); },
        [this, player](const std::string& op) { this->processOption(player, op, 120); return true; },
        getPlaceName(), getHeaderColor(), getASCIIArt()
    );
}

std::string NPCAlchemist::getPlaceName() const {
    return "LABORATORIO DE ALQUIMIA";
}

Color NPCAlchemist::getHeaderColor() const {
    return Color::GREEN;
}

Color NPCAlchemist::getArtColor() const {
    return Color::GREEN;
}

const std::vector<std::string>& NPCAlchemist::getASCIIArt() const {
    return NPCAlquimistaLayouts::arteAlquimista;
}

std::vector<std::string> NPCAlchemist::getDialogue(Character* /*jogador*/) {
    std::vector<std::string> lines = {
        "Seja bem-vindo ao laboratorio de transmutacao!",
        "Eu sou Quintus, o Alchemist Real. Se voce me trouxer ingredientes de monstros",
        "e alimentos terrestres, posso transmutar elixires poderosos!",
        "Minhas criacoes podem heal sua alma ou devastar as defesas inimigas."
    };
    return lines;
}

std::vector<std::string> NPCAlchemist::getMenuOptions(Character* /*jogador*/, int /*larguraTerminal*/) {
    return {
        "Pocao de Cura Grande (50%VM) [1x Maca + 1x Po magico]",
        "Pocao de Forca Alquimica [1x Pao + 1x Dente de goblin]",
        "Pocao de Veneno Alquimica [1x Carne Seca + 1x Gosma acida]",
        "Pocao de Lentidao Alquimica [1x Queijo + 1x Nucleo pegajoso]",
        "Voltar"
    };
}

void NPCAlchemist::processOption(Character* player, const std::string& option, int /*larguraTerminal*/) {
    std::string foodReq = "";
    std::string dropReq = "";
    ItemID productId = ItemID::None;

    if (option.find("Cura Grande") != std::string::npos) {
        foodReq = "Maca";
        dropReq = "Po magico";
        productId = ItemID::PocaoCuraGrande;
    }
    else if (option.find("Forca Alquimica") != std::string::npos) {
        foodReq = "Pao";
        dropReq = "Dente de goblin";
        productId = ItemID::PocaoForcaAlquimica;
    }
    else if (option.find("Veneno Alquimica") != std::string::npos) {
        foodReq = "Carne Seca";
        dropReq = "Gosma acida";
        productId = ItemID::PocaoVenenoAlquimica;
    }
    else if (option.find("Lentidao Alquimica") != std::string::npos) {
        foodReq = "Queijo";
        dropReq = "Nucleo pegajoso";
        productId = ItemID::PocaoLentidaoAlquimica;
    }

    if (productId != ItemID::None) {
        auto* inventory = player->getInventory();
        int foodCount = inventory->getItemCount(foodReq);
        int dropCount = inventory->getItemCount(dropReq);

        if (foodCount >= 1 && dropCount >= 1) {
            inventory->removeItem(foodReq);
            inventory->removeItem(dropReq);

            auto newItem = ItemFactory::createItem(productId);
            if (newItem) {
                std::string productName = newItem->getItemName();
                inventory->addItem(std::move(newItem));

                std::vector<std::string> successMsg = {
                    "Mistura fervilhando... Vapor borbulhando...",
                    "Sucesso! VocÃª obteve: " + productName
                };
            }
        } else {
            std::vector<std::string> errorMsg = {
                "Ingredientes insuficientes!",
                "Voce precisa de 1x " + foodReq + " e 1x " + dropReq + " para esta pocao."
            };
        }
    }
}
