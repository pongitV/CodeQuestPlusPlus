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
    std::map<int, StoreProduct> estoquePocoes = {
        {1, {ItemID::PocaoCura30, 10, -1}}
    };

    std::map<int, StoreProduct> estoqueTalismas = {
        {1, {ItemID::TalismaUrso, 200, 1}},
        {2, {ItemID::TalismaCorvo, 200, 1}},
        {3, {ItemID::TalismaLeopardo, 200, 1}},
        {4, {ItemID::TalismaCoruja, 200, 1}}
    };

    std::map<int, StoreProduct> estoqueIguarias = {
        {1, {ItemID::DispositivoLinguagem, 1000, 1}}
    };

    // --- APARENCIA E DIALOGOS ---
    void processPurchasePocoes(Character* currentPlayer);
    void processPurchaseTalismas(Character* currentPlayer);
    void processPurchaseIguarias(Character* currentPlayer);
    void processarVendaDeItens(Character* currentPlayer);

    void dialogoFranchesco(const std::vector<std::string>& linhas) {
    }
    
    void dialogoFranchescoUnico(const std::string& msg) {
        InputControl::lerSelecaoMenuEmPopup("Franchesco", {msg}, {"OK"}, Color::CYAN, NPCMerchantLayouts::arteMerchant);
    }
}

// --- INFORMACOES DO LUGAR ---
std::string NPCMerchant::getNameDoLugar() const {
    return "MERCADOR AMBULANTE";
}

Color NPCMerchant::obterCorDoCabecalho() const {
    return Color::YELLOW;
}

Color NPCMerchant::obterCorDaArte() const {
    return Color::YELLOW;
}

const std::vector<std::string>& NPCMerchant::obterArteASCII() const {
    return NPCMerchantLayouts::arteMerchant;
}

// --- INTERACAO E MENU ---
void NPCMerchant::interagir(Character* jogador) {
    InputControl::executarLoopMenuPopup(
        [this, jogador]() { return this->obterDialogo(jogador); },
        [this, jogador]() { return this->obterOpcoesMenu(jogador, 120); },
        [this, jogador](const std::string& op) { this->processarOpcao(jogador, op, 120); return true; },
        getNameDoLugar(), obterCorDoCabecalho(), obterArteASCII()
    );
}

std::vector<std::string> NPCMerchant::obterDialogo(Character* /*jogador*/) {
    return std::vector<std::string>{
        "Bem-vindo! De uma olhada nas",
        "minhas mercadorias."
    };
}

std::vector<std::string> NPCMerchant::obterOpcoesMenu(Character* jogador, int larguraDoTerminal) {
    return {
        "COMPRAR Pocoes",
        "COMPRAR Talismas",
        "COMPRAR Iguarias",
        "VENDER Itens do Inventory",
        "Missoes de Franchesco",
        "VOLTAR"
    };
}

void NPCMerchant::processarOpcao(Character* jogador, const std::string& opcao, int larguraDoTerminal) {
    if (opcao == "COMPRAR Pocoes") {
        processPurchasePocoes(jogador);
    }
    else if (opcao == "COMPRAR Talismas") {
        processPurchaseTalismas(jogador);
    }
    else if (opcao == "COMPRAR Iguarias") {
        processPurchaseIguarias(jogador);
    }
    else if (opcao == "VENDER Itens do Inventory") {
        processarVendaDeItens(jogador);
    }
    else if (opcao == "Missoes de Franchesco") {
        NPCInteraction::processarMenuMissoesVazio(jogador, "MISSOES DE FRANCHESCO", Color::YELLOW, "Franchesco", "Ah, meu amigo! Nao tenho nenhum pedido especial para voce agora.");
    }
}

namespace {
    // --- PROCESSAMENTO DE OPCOES ---
    void processPurchasePocoes(Character* currentPlayer) {
        Store::processPurchase(currentPlayer, "LOJA - POCOES", Color::YELLOW, estoquePocoes, 
            [](const std::string& msg) { dialogoFranchescoUnico(msg); }, NPCInteraction::obterFormatadorStatusItem, NPCMerchantLayouts::arteMerchant);
    }

    void processPurchaseTalismas(Character* currentPlayer) {
        Store::processPurchase(currentPlayer, "LOJA - TALISMAS", Color::YELLOW, estoqueTalismas, 
            [](const std::string& msg) { dialogoFranchescoUnico(msg); }, NPCInteraction::obterFormatadorStatusItem, NPCMerchantLayouts::arteMerchant);
    }

    void processPurchaseIguarias(Character* currentPlayer) {
        Store::processPurchase(currentPlayer, "LOJA - IGUARIAS", Color::YELLOW, estoqueIguarias, 
            [](const std::string& msg) { dialogoFranchescoUnico(msg); }, NPCInteraction::obterFormatadorStatusItem, NPCMerchantLayouts::arteMerchant);
    }

    void processarVendaDeItens(Character* currentPlayer) {
        do {
            std::vector<std::pair<std::string, std::vector<Item*>>> gruposItens;
            std::map<std::string, int> indexMap;
            
            for (auto* item : currentPlayer->obterInventario()->obterTodosOsItens()) {
                if (item->obterTipo() != TipoEquipamento::MISSAO) {
                    std::string nomeItem = item->getNameItem();
                    bool equipado = currentPlayer->isItemEquipado(item);
                    std::string chave = nomeItem;
                    if (equipado) {
                        chave += " [Equipado]";
                    }
                    
                    if (indexMap.find(chave) == indexMap.end()) {
                        indexMap[chave] = gruposItens.size();
                        gruposItens.push_back({chave, {item}});
                    } else {
                        gruposItens[indexMap[chave]].second.push_back(item);
                    }
                }
            }

            std::vector<std::vector<Item*>> itensValidos;
            std::vector<std::string> opcoesItem;
            
            for (const auto& grupo : gruposItens) {
                int qtd = grupo.second.size();
                Item* itemExemplo = grupo.second.front();
                int priceVenda = itemExemplo->obterPrecoVenda();
                
                std::string textoOpcao = grupo.first + " (" + std::to_string(priceVenda) + "G)";
                if (qtd > 1) {
                    textoOpcao += " x" + std::to_string(qtd);
                }
                
                itensValidos.push_back(grupo.second);
                opcoesItem.push_back(textoOpcao);
            }

            if (opcoesItem.empty()) { dialogoFranchescoUnico("Voce nao tem nada que me interesse!"); break; }
            opcoesItem.push_back("VOLTAR");
            
            int escolha = InputControl::lerSelecaoMenuEmPopup("VENDER ITENS", {"Seu Ouro: " + std::to_string(currentPlayer->obterInventario()->obterOuro()) + "G", "Escolha um item para vender:"}, opcoesItem, Color::YELLOW, NPCMerchantLayouts::arteMerchant);
            if (escolha == -1 || escolha == static_cast<int>(opcoesItem.size()) - 1) break;
            
            std::vector<Item*> itensEscolhidos = itensValidos[escolha];
            Item* itemParaVenda = itensEscolhidos.front();

            if (currentPlayer->isItemEquipado(itemParaVenda)) {
                dialogoFranchescoUnico("Nao e possivel vender itens que estao equipados!");
                continue;
            }
            
            int qtdParaVender = 1;
            if (itensEscolhidos.size() > 1) {
                std::vector<std::string> opcoesQtd = {
                    "Vender 1 unidade",
                    "Vender Todos (" + std::to_string(itensEscolhidos.size()) + " unidades)",
                    "Digitar amount...",
                    "Cancelar"
                };
                
                int escolhaQtd = InputControl::lerSelecaoMenuEmPopup(
                    "QUANTIDADE: " + itemParaVenda->getNameItem(),
                    {"Voce possui " + std::to_string(itensEscolhidos.size()) + " unidades deste item."},
                    opcoesQtd, 
                    Color::YELLOW, 
                    NPCMerchantLayouts::arteMerchant
                );
                
                if (escolhaQtd == 0) {
                    qtdParaVender = 1;
                } else if (escolhaQtd == 1) {
                    qtdParaVender = itensEscolhidos.size();
                } else if (escolhaQtd == 2) {
                    std::string msgQtd = "Quantidade (1 a " + std::to_string(itensEscolhidos.size()) + ", 0 cancelar): ";
                    qtdParaVender = 0; // Not supported anymore
                } else {
                    continue; // Cancelar
                }
            }
            
            if (qtdParaVender == 0) continue;

            std::string nomeItemVenda = itemParaVenda->getNameItem();
            int priceVendaUnitario = itemParaVenda->obterPrecoVenda();
            int ganhoTotal = priceVendaUnitario * qtdParaVender;
            
            currentPlayer->obterInventario()->adicionarOuro(ganhoTotal);
            for (int i = 0; i < qtdParaVender; ++i) {
                currentPlayer->obterInventario()->removerItem(itensEscolhidos[i]);
            }
            
            if (qtdParaVender > 1) {
                dialogoFranchescoUnico("Voce vendeu " + std::to_string(qtdParaVender) + "x " + nomeItemVenda + " por " + std::to_string(ganhoTotal) + "G!");
            } else {
                dialogoFranchescoUnico("Voce vendeu " + nomeItemVenda + " por " + std::to_string(ganhoTotal) + "G!");
            }
        } while (true);
    }
}
