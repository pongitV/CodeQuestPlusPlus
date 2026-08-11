#include "NPCInteraction.h"
#include <iostream>
#include "../../core/utils/InputControl.h"
#include "../../ui/screens/inventory/ScreenInventory.h"
#include "../../systems/inventory/ItemFactory.h"
#include "../../core/utils/DialogFunctions.h"
#include "../../ui/screens/ScreenBase.h"
#include "../../core/utils/Color.h"

// --- INTERACAO PRINCIPAL ---
void NPCInteraction::interagir(Character* currentPlayer) {
    std::string opcao;
    
    InputControl::executarLoopMenuPopup(
        [this, currentPlayer]() { return this->obterDialogo(currentPlayer); },
        [this, currentPlayer]() { return this->obterOpcoesMenu(currentPlayer, 120); },
        [this, currentPlayer](const std::string& op) { this->processarOpcao(currentPlayer, op, 120); return true; },
        getNameDoLugar(), obterCorDoCabecalho(), obterArteASCII()
    );
}

void NPCInteraction::processarMenuMissoesVazio(Character* currentPlayer, const std::string& tituloMenu, Color corCabecalho, const std::string& nomeNPC, const std::string& falaVazia) {
    std::string opcaoMissao;
    do {
        std::vector<std::string> missoes = {
            "(Nenhuma missao disponivel)",
            "VOLTAR"
        };
        
        int id = InputControl::lerSelecaoMenuEmPopup(tituloMenu, {"Escolha uma missao:"}, missoes, corCabecalho);
        if (id == -1) break;
        opcaoMissao = missoes[id];

        if (opcaoMissao == "(Nenhuma missao disponivel)") {
        }
    } while (opcaoMissao != "VOLTAR");
}

bool NPCInteraction::verificarMaterialNoInventario(Character* currentPlayer, const std::string& nomeMaterial, int quantidadeNecessaria, const std::string& nomeNPC, Color corNPC, const std::string& mensagemPersonalizada) {
    int qtdAtual = currentPlayer->getInventory()->obterQuantidadeItem(nomeMaterial);
    if (qtdAtual < quantidadeNecessaria) {
        std::string texto = mensagemPersonalizada.empty()
            ? "Voce precisa de " + std::to_string(quantidadeNecessaria) + "x " + nomeMaterial + " para isso!"
            : mensagemPersonalizada;
        InputControl::lerSelecaoMenuEmPopup(nomeNPC, {texto}, {"OK"}, corNPC);
        return false;
    }
    return true;
}

Item* NPCInteraction::lerItemDoInventario(Character* currentPlayer, const std::string& mensagemDialogo, const std::string& nomeNPC, Color corNPC, std::string& codigoSaida, bool displayPrecos) {
    Item* itemSelecionado = nullptr;

    TelaBase::executarLoop(
        [](bool animar) { TelaInventario::displayCabecalhoInventario(animar); },
        [&]() {
        },
        [currentPlayer, displayPrecos]() {
            std::vector<std::string> opcoes;
            opcoes.push_back("Arsenal de Equipamentos");
            opcoes.push_back("Itens Consumiveis");
            opcoes.push_back("Estoque e Materiais");
            opcoes.push_back("Itens de Missao");
            opcoes.push_back("VOLTAR");
            return opcoes;
        },
        [&](int escolhaCat) {
            if (escolhaCat < 0 || escolhaCat == 4) {
                codigoSaida = "0";
                return false;
            }
            
            auto itensCategoria = TelaInventario::obterListaCategoria(currentPlayer, escolhaCat, displayPrecos);
            
            TelaBase::executarLoop(
                [](bool animar) { TelaInventario::displayCabecalhoInventario(animar); },
                [&]() {},
                [&itensCategoria]() {
                    std::vector<std::string> opcoes;
                    for (auto& par : itensCategoria) opcoes.push_back(par.first);
                    opcoes.push_back("VOLTAR");
                    return opcoes;
                },
                [&](int escolhaItem) {
                    if (escolhaItem < 0 || escolhaItem >= static_cast<int>(itensCategoria.size())) {
                        return false;
                    }
                    itemSelecionado = itensCategoria[escolhaItem].second;
                    codigoSaida = "selecionado";
                    return false;
                }
            );

            if (itemSelecionado) return false;
            return true;
        }
    );
    
    return itemSelecionado;
}

void NPCInteraction::displayTelaDeSucesso(const std::string& tituloCabecalho, Color corCabecalho, const std::string& equacao, const std::vector<std::string>& arteAscii, const std::string& nomeNPC, const std::string& falaNPC) {
    InputControl::lerSelecaoMenuEmPopup(tituloCabecalho, {falaNPC}, {"OK"}, corCabecalho, arteAscii);
}

std::string NPCInteraction::obterFormatadorStatusItem(ItemID id) {
    std::unique_ptr<Item> tempItem = ItemFactory::criarItem(id);
    return tempItem ? tempItem->obterInfoStatus() : "";
}

bool NPCInteraction::verificarItemNaoEquipado(Character* currentPlayer, Item* itemAvaliado, const std::string& nomeNPC, Color corNPC, const std::string& msgErro) {
    if (currentPlayer->isItemEquipado(itemAvaliado)) {
        InputControl::lerSelecaoMenuEmPopup(nomeNPC, {msgErro}, {"OK"}, corNPC);
        return false;
    }
    return true;
}
