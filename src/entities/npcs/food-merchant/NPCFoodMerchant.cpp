#include "NPCFoodMerchant.h"
#include "NPCFoodMerchantLayout.h"
#include "../../../core/state/GameMenu.h"
#include "../../../core/utils/InputControl.h"
#include "../../../systems/inventory/ItemFactory.h"
#include <iostream>
#include "../../../core/utils/Color.h"

void NPCFoodMerchant::interact(Character* player) {
    NPCInteraction::interact(player);
}

std::string NPCFoodMerchant::getPlaceName() const {
    return "FEIRA DO REINO";
}

Color NPCFoodMerchant::getHeaderColor() const {
    return Color::GREEN_CLARO;
}

Color NPCFoodMerchant::getArtColor() const {
    return Color::GREEN_CLARO;
}

const std::vector<std::string>& NPCFoodMerchant::getASCIIArt() const {
    return NPCFoodMerchantLayouts::foodMerchantArt;
}

std::vector<std::string> NPCFoodMerchant::getDialogue(Character* /*jogador*/) {
    std::vector<std::string> lines = {
        "Olá, combatente! Sente fome? A jornada deve ser cansativa.",
        "Tenho as melhores e mais frescas provisões do reino!",
        "Nossos alimentos curam sua health instantaneamente ao serem consumidos na mochila."
    };
    return lines;
}

std::vector<std::string> NPCFoodMerchant::getMenuOptions(Character* /*jogador*/, int /*larguraTerminal*/) {
    return {
        "Maca (Cura 15 HP) - 5G",
        "Pao (Cura 25 HP) - 10G",
        "Queijo (Cura 40 HP) - 18G",
        "Carne Seca (Cura 60 HP) - 30G",
        "Voltar"
    };
}

void NPCFoodMerchant::processOption(Character* player, const std::string& option, int /*larguraTerminal*/) {
    ItemID purchaseId = ItemID::None;
    int cost = 0;

    if (option.find("Maca") != std::string::npos) {
        purchaseId = ItemID::Maca;
        cost = 5;
    }
    else if (option.find("Pao") != std::string::npos) {
        purchaseId = ItemID::Pao;
        cost = 10;
    }
    else if (option.find("Queijo") != std::string::npos) {
        purchaseId = ItemID::Queijo;
        cost = 18;
    }
    else if (option.find("Carne Seca") != std::string::npos) {
        purchaseId = ItemID::CarneSeca;
        cost = 30;
    }

    if (purchaseId != ItemID::None) {
        if (player->getInventory()->getGold() >= cost) {
            auto item = ItemFactory::createItem(purchaseId);
            if (item) {
                player->getInventory()->addGold(-cost);
                std::string itemName = item->getItemName();
                player->getInventory()->addItem(std::move(item));
            }
        } else {
        }
    }
}
