#include "NPCFoodMerchant.h"
#include "NPCFoodMerchantLayout.h"
#include "../../../core/state/GameMenu.h"
#include "../../../core/utils/InputControl.h"
#include "../../../systems/inventory/ItemFactory.h"
#include <iostream>
#include "../../../core/utils/Color.h"

void NPCFoodMerchant::interagir(Character* jogador) {
    NPCInteraction::interagir(jogador);
}

std::string NPCFoodMerchant::getNameDoLugar() const {
    return "FEIRA DO REINO";
}

Color NPCFoodMerchant::obterCorDoCabecalho() const {
    return Color::GREEN_CLARO;
}

Color NPCFoodMerchant::obterCorDaArte() const {
    return Color::GREEN_CLARO;
}

const std::vector<std::string>& NPCFoodMerchant::obterArteASCII() const {
    return NPCFoodMerchantLayouts::arteFoodMerchant;
}

std::vector<std::string> NPCFoodMerchant::obterDialogo(Character* /*jogador*/) {
    std::vector<std::string> linhas = {
        "Olá, combatente! Sente fome? A jornada deve ser cansativa.",
        "Tenho as melhores e mais frescas provisões do reino!",
        "Nossos alimentos curam sua health instantaneamente ao serem consumidos na mochila."
    };
    return linhas;
}

std::vector<std::string> NPCFoodMerchant::obterOpcoesMenu(Character* jogador, int /*larguraDoTerminal*/) {
    return {
        "Maca (Cura 15 HP) - 5G",
        "Pao (Cura 25 HP) - 10G",
        "Queijo (Cura 40 HP) - 18G",
        "Carne Seca (Cura 60 HP) - 30G",
        "Voltar"
    };
}

void NPCFoodMerchant::processarOpcao(Character* jogador, const std::string& opcao, int /*larguraDoTerminal*/) {
    ItemID idCompra = ItemID::Nenhum;
    int custo = 0;

    if (opcao.find("Maca") != std::string::npos) {
        idCompra = ItemID::Maca;
        custo = 5;
    }
    else if (opcao.find("Pao") != std::string::npos) {
        idCompra = ItemID::Pao;
        custo = 10;
    }
    else if (opcao.find("Queijo") != std::string::npos) {
        idCompra = ItemID::Queijo;
        custo = 18;
    }
    else if (opcao.find("Carne Seca") != std::string::npos) {
        idCompra = ItemID::CarneSeca;
        custo = 30;
    }

    if (idCompra != ItemID::Nenhum) {
        if (jogador->obterInventario()->obterOuro() >= custo) {
            auto item = ItemFactory::criarItem(idCompra);
            if (item) {
                jogador->obterInventario()->adicionarOuro(-custo);
                std::string nomeItem = item->getNameItem();
                jogador->obterInventario()->adicionarItem(std::move(item));
            }
        } else {
        }
    }
}
