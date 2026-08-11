#include "NPCAlchemist.h"
#include "NPCAlchemistLayout.h"
#include "../../../core/state/GameMenu.h"
#include "../../../core/utils/InputControl.h"
#include "../../../systems/inventory/ItemFactory.h"
#include <iostream>
#include "../../../core/utils/Color.h"

void NPCAlchemist::interagir(Character* jogador) {
    InputControl::executarLoopMenuPopup(
        [this, jogador]() { return this->obterDialogo(jogador); },
        [this, jogador]() { return this->obterOpcoesMenu(jogador, 120); },
        [this, jogador](const std::string& op) { this->processarOpcao(jogador, op, 120); return true; },
        getNameDoLugar(), obterCorDoCabecalho(), obterArteASCII()
    );
}

std::string NPCAlchemist::getNameDoLugar() const {
    return "LABORATORIO DE ALQUIMIA";
}

Color NPCAlchemist::obterCorDoCabecalho() const {
    return Color::GREEN;
}

Color NPCAlchemist::obterCorDaArte() const {
    return Color::GREEN;
}

const std::vector<std::string>& NPCAlchemist::obterArteASCII() const {
    return NPCAlchemistLayouts::arteAlchemist;
}

std::vector<std::string> NPCAlchemist::obterDialogo(Character* /*jogador*/) {
    std::vector<std::string> linhas = {
        "Seja bem-vindo ao laboratorio de transmutacao!",
        "Eu sou Quintus, o Alchemist Real. Se voce me trouxer ingredientes de monstros",
        "e alimentos terrestres, posso transmutar elixires poderosos!",
        "Minhas criacoes podem heal sua alma ou devastar as defesas inimigas."
    };
    return linhas;
}

std::vector<std::string> NPCAlchemist::obterOpcoesMenu(Character* jogador, int /*larguraDoTerminal*/) {
    return {
        "Pocao de Cura Grande (50%VM) [1x Maca + 1x Po magico]",
        "Pocao de Forca Alquimica [1x Pao + 1x Dente de goblin]",
        "Pocao de Veneno Alquimica [1x Carne Seca + 1x Gosma acida]",
        "Pocao de Lentidao Alquimica [1x Queijo + 1x Nucleo pegajoso]",
        "Voltar"
    };
}

void NPCAlchemist::processarOpcao(Character* jogador, const std::string& opcao, int /*larguraDoTerminal*/) {
    std::string comidaReq = "";
    std::string dropReq = "";
    ItemID produtoId = ItemID::Nenhum;

    if (opcao.find("Cura Grande") != std::string::npos) {
        comidaReq = "Maca";
        dropReq = "Po magico";
        produtoId = ItemID::PocaoCuraGrande;
    }
    else if (opcao.find("Forca Alquimica") != std::string::npos) {
        comidaReq = "Pao";
        dropReq = "Dente de goblin";
        produtoId = ItemID::PocaoForcaAlquimica;
    }
    else if (opcao.find("Veneno Alquimica") != std::string::npos) {
        comidaReq = "Carne Seca";
        dropReq = "Gosma acida";
        produtoId = ItemID::PocaoVenenoAlquimica;
    }
    else if (opcao.find("Lentidao Alquimica") != std::string::npos) {
        comidaReq = "Queijo";
        dropReq = "Nucleo pegajoso";
        produtoId = ItemID::PocaoLentidaoAlquimica;
    }

    if (produtoId != ItemID::Nenhum) {
        auto* mochila = jogador->obterInventario();
        int qtdFoodMerchant = mochila->contarItem(comidaReq);
        int qtdDrop = mochila->contarItem(dropReq);

        if (qtdFoodMerchant >= 1 && qtdDrop >= 1) {
            mochila->removerItem(comidaReq);
            mochila->removerItem(dropReq);

            auto itemNovo = ItemFactory::criarItem(produtoId);
            if (itemNovo) {
                std::string nomeProduto = itemNovo->getNameItem();
                mochila->adicionarItem(std::move(itemNovo));

                std::vector<std::string> msgSucesso = {
                    "Mistura fervilhando... Vapor borbulhando...",
                    "Sucesso! VocÃª obteve: " + nomeProduto
                };
            }
        } else {
            std::vector<std::string> msgErro = {
                "Ingredientes insuficientes!",
                "VocÃª precisa de:",
                " -> 1x " + comidaReq + " (Possui: " + std::to_string(qtdFoodMerchant) + ")",
                " -> 1x " + dropReq + " (Possui: " + std::to_string(qtdDrop) + ")"
            };
        }
    }
}

