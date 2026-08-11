#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <algorithm>
#include <memory>

#include "NPCBlacksmith.h"
#include "../../../systems/inventory/Item.h"
#include "../../../systems/inventory/ItemFactory.h"
#include "../../../systems/inventory/equipment/ArmorEquipment.h"
#include "../../../systems/inventory/equipment/ShieldEquipment.h"
#include "../../../ui/screens/inventory/ScreenInventory.h"
#include "../../../core/utils/InputControl.h"
#include "../../../core/state/Store.h"
#include "../../../core/utils/DialogFunctions.h"
#include "NPCBlacksmithLayout.h"
#include "../../../core/utils/Color.h"

namespace {
    // --- DADOS DO ESTOQUE ---
    std::map<int, StoreProduct> estoqueArmas = {
        {1, {ItemID::EspadaFerro, 40, -1}},
        {2, {ItemID::ArcoMadeira, 40, -1}},
        {3, {ItemID::CajadoCristal, 40, -1}},
        {4, {ItemID::ViolaoEncantado, 40, -1}}
    };
    
    std::map<int, StoreProduct> estoqueArmaduras = {
        {1, {ItemID::ArmaduraMalha, 40, -1}},
        {2, {ItemID::ArmaduraCouro, 40, -1}},
        {3, {ItemID::Tunica, 40, -1}},
        {4, {ItemID::TrajeNobre, 40, -1}}
    };
    
    void processPurchaseDeEquipamento(Character* currentPlayer, bool comprandoArmas);
    void processarMelhoriaNaBigorna(Character* currentPlayer);
    void processarUpgradePorMaterial(Character* currentPlayer);
    void processarConsertoDeEscudo(Character* currentPlayer);

    // --- APARENCIA E DIALOGOS ---
    void dialogoBjorn(const std::vector<std::string>& linhas) {
    }
    
    void dialogoBjornUnico(const std::string& msg) {
        InputControl::lerSelecaoMenuEmPopup("Bjorn", {msg}, {"OK"}, Color::CYAN, NPCBlacksmithLayouts::arteBlacksmith);
    }
}

// --- INFORMACOES DO LUGAR ---
std::string NPCBlacksmith::getNameDoLugar() const {
    return "FORJA DO BJORN";
}

Color NPCBlacksmith::obterCorDoCabecalho() const {
    return Color::CYAN;
}

Color NPCBlacksmith::obterCorDaArte() const {
    return Color::CYAN;
}

const std::vector<std::string>& NPCBlacksmith::obterArteASCII() const {
    return NPCBlacksmithLayouts::arteBlacksmith;
}

// --- INTERACAO E MENU ---
void NPCBlacksmith::interagir(Character* jogador) {
    InputControl::executarLoopMenuPopup(
        [this, jogador]() { return this->obterDialogo(jogador); },
        [this, jogador]() { return this->obterOpcoesMenu(jogador, 120); },
        [this, jogador](const std::string& op) { this->processarOpcao(jogador, op, 120); return true; },
        getNameDoLugar(), obterCorDoCabecalho(), obterArteASCII()
    );
}

std::vector<std::string> NPCBlacksmith::obterDialogo(Character* /*jogador*/) {
    return std::vector<std::string>{
        "Bem-vindo a minha forja, salvador!",
        "O que vai ser hoje?"
    };
}

std::vector<std::string> NPCBlacksmith::obterOpcoesMenu(Character* /*jogador*/, int /*larguraDoTerminal*/) {
    return {
        "COMPRAR Armas das Classes",
        "COMPRAR Armaduras das Classes",
        "MELHORAR POR FUSAO",
        "MELHORAR POR MATERIAL",
        "Missoes de Bjorn",
        "VOLTAR"
    };
}

void NPCBlacksmith::processarOpcao(Character* jogador, const std::string& opcao, int /*larguraDoTerminal*/) {
    if (opcao == "COMPRAR Armas das Classes" || opcao == "COMPRAR Armaduras das Classes") {
        processPurchaseDeEquipamento(jogador, opcao == "COMPRAR Armas das Classes");
    } else if (opcao == "MELHORAR POR FUSAO") {
        processarMelhoriaNaBigorna(jogador);
    } else if (opcao == "MELHORAR POR MATERIAL") {
        processarUpgradePorMaterial(jogador);
    } else if (opcao == "CONSERTAR Shield") {
        processarConsertoDeEscudo(jogador);
    } else if (opcao == "Missoes de Bjorn") {
        NPCInteraction::processarMenuMissoesVazio(jogador, "MISSOES DE BJORN", Color::CYAN, "Bjorn", "Nao tenho nenhum trabalho especial para voce no momento.");
    }
}

namespace {
    // --- PROCESSAMENTO DE OPCOES ---
    void processPurchaseDeEquipamento(Character* currentPlayer, bool comprandoArmas) {
        auto& estoqueAtual = comprandoArmas ? estoqueArmas : estoqueArmaduras;
        std::string tituloStore = comprandoArmas ? "FORJA - ARMAS" : "FORJA - ARMADURAS";

        Store::processPurchase(currentPlayer, tituloStore, Color::CYAN, estoqueAtual, 
            [](const std::string& msg) { dialogoBjornUnico(msg); }, NPCInteraction::obterFormatadorStatusItem, NPCBlacksmithLayouts::arteBlacksmith);
    }

    void processarMelhoriaNaBigorna(Character* currentPlayer) {
        do {
            std::vector<Item*> itensValidos;
            std::vector<std::string> opcoesItem;
            for (auto* item : currentPlayer->obterInventario()->obterTodosOsItens()) {
                TipoEquipamento tipo = item->obterTipo();
                if ((tipo == TipoEquipamento::ARMA || tipo == TipoEquipamento::ESCUDO || tipo == TipoEquipamento::ARMADURA) && !item->temPropriedade(Propriedade::Melhorado)) {
                    itensValidos.push_back(item);
                    opcoesItem.push_back(item->getNameItem());
                }
            }
            if (opcoesItem.empty()) { dialogoBjornUnico("Voce nao tem nenhum equipamento que eu possa melhorar!"); break; }
            opcoesItem.push_back("VOLTAR");
            
            int escolha = InputControl::lerSelecaoMenuEmPopup("FUSAO DE EQUIPAMENTO", {"Qual item deseja fundir? (Requer copia no inventory)"}, opcoesItem, Color::CYAN, NPCBlacksmithLayouts::arteBigorna);
            if (escolha == -1 || escolha == static_cast<int>(opcoesItem.size()) - 1) break;
            
            Item* itemBase = itensValidos[escolha];
            if (!NPCInteraction::verificarItemNaoEquipado(currentPlayer, itemBase, "Bjorn", Color::CYAN, "Voce precisa DESEQUIPAR o item antes de usa-lo na bigorna!")) continue;

            if (!NPCInteraction::verificarMaterialNoInventario(currentPlayer, itemBase->getNameItem(), 2, "Bjorn", Color::CYAN)) continue;

            if ((currentPlayer->obterArma() && currentPlayer->obterArma()->getNameItem() == itemBase->getNameItem()) ||
                (currentPlayer->obterEscudo() && currentPlayer->obterEscudo()->getNameItem() == itemBase->getNameItem()) ||
                (currentPlayer->obterArmadura() && currentPlayer->obterArmadura()->getNameItem() == itemBase->getNameItem())) {
                dialogoBjornUnico("Voce possui uma copia deste item equipada! DESEQUIPE antes de fundir."); continue;
            }

            std::unique_ptr<Item> novoItem = itemBase->gerarCopiaMelhorada();

             if (novoItem) {
                std::string nomeAntigo = itemBase->getNameItem();
                std::string novoNome = novoItem->getNameItem();
                currentPlayer->obterInventario()->removerItem(itemBase);
                currentPlayer->obterInventario()->removerItem(nomeAntigo);
                currentPlayer->obterInventario()->adicionarItem(std::move(novoItem));

                std::string equacao = "[" + nomeAntigo + "] + [" + nomeAntigo + "] = [" + novoNome + "]";
            }
        } while (true);
    }

    void processarUpgradePorMaterial(Character* currentPlayer) {
        std::string nomePedraUpgrade = ItemFactory::getNameDeID(ItemID::PedraUpgrade);
        do {
            if (!NPCInteraction::verificarMaterialNoInventario(currentPlayer, nomePedraUpgrade, 1, "Bjorn", Color::CYAN)) {
                return;
            }
            
            std::vector<Item*> itensValidos;
            std::vector<std::string> opcoesItem;
            for (auto* item : currentPlayer->obterInventario()->obterTodosOsItens()) {
                if (item->obterTipo() == TipoEquipamento::ARMADURA && !item->temPropriedade(Propriedade::MelhoradoMaterial)) {
                    itensValidos.push_back(item);
                    opcoesItem.push_back(item->getNameItem());
                }
            }
            if (opcoesItem.empty()) { dialogoBjornUnico("Voce nao tem armaduras validas para imbuir!"); break; }
            opcoesItem.push_back("VOLTAR");
            
            int escolha = InputControl::lerSelecaoMenuEmPopup("IMBUIR ARMADURA", {"Qual armadura imbuir com a Pedra? (+3 Defesa)"}, opcoesItem, Color::CYAN, NPCBlacksmithLayouts::arteBigorna);
            if (escolha == -1 || escolha == static_cast<int>(opcoesItem.size()) - 1) break;

            Item* itemParaUpgrade = itensValidos[escolha];
            if (!NPCInteraction::verificarItemNaoEquipado(currentPlayer, itemParaUpgrade, "Bjorn", Color::CYAN, "Voce precisa DESEQUIPAR o item antes de usa-lo na bigorna!")) continue;

            EquipamentoArmadura* armadura = dynamic_cast<EquipamentoArmadura*>(itemParaUpgrade);
            if (!armadura) continue;

            if (armadura->temPropriedade(Propriedade::MelhoradoMaterial)) {
                dialogoBjornUnico("Esta armadura ja foi imbuida com a pedra magica!");
                continue;
            }

            std::string nomeAntigo = armadura->getNameItem();
            std::string novoNome = nomeAntigo + " (Imbuida)";

            auto novaArmadura = std::make_unique<EquipamentoArmadura>(
                novoNome, 
                armadura->obterReducaoFixa() + 3, 
                armadura->obterReqResistencia(), 
                armadura->obterReqConstituicao(), 
                armadura->obterPrecoVenda() + 200
            );

            for (Propriedade prop : armadura->obterPropriedades()) novaArmadura->adicionarPropriedade(prop);
            novaArmadura->adicionarPropriedade(Propriedade::MelhoradoMaterial);

            currentPlayer->obterInventario()->removerItem(nomePedraUpgrade);
            currentPlayer->obterInventario()->removerItem(armadura);
            currentPlayer->obterInventario()->adicionarItem(std::move(novaArmadura));

            std::string equacao = "[" + nomeAntigo + "] + [Pedra magica] = [" + novoNome + "]";
        } while (true);
    }

    void processarConsertoDeEscudo(Character* currentPlayer) {
        do {
            std::vector<EquipamentoEscudo*> escudosDanificados;
            std::vector<std::string> opcoesEscudo;

            for (auto* item : currentPlayer->obterInventario()->obterTodosOsItens()) {
                EquipamentoEscudo* escudo = dynamic_cast<EquipamentoEscudo*>(item);
                if (escudo && escudo->obterDurabilidadeAtualEscudo() < escudo->obterDurabilidadeMaxima()) {
                    escudosDanificados.push_back(escudo);
                    int custo = (escudo->obterDurabilidadeMaxima() - escudo->obterDurabilidadeAtualEscudo()) * 5;
                    opcoesEscudo.push_back(escudo->getNameItem() + " (" + std::to_string(escudo->obterDurabilidadeAtualEscudo()) + "/" + std::to_string(escudo->obterDurabilidadeMaxima()) + ") - " + std::to_string(custo) + "g");
                }
            }

            if (escudosDanificados.empty()) {
                dialogoBjornUnico("Voce nao tem nenhum escudo danificado que eu possa consertar!");
                break;
            }
            
            opcoesEscudo.push_back("VOLTAR");
            
            int escolha = InputControl::lerSelecaoMenuEmPopup("CONSERTAR ESCUDO", {"Qual escudo deseja consertar? (5g por ponto perdido)"}, opcoesEscudo, Color::CYAN, NPCBlacksmithLayouts::arteBigorna);
            if (escolha == -1 || escolha == static_cast<int>(opcoesEscudo.size()) - 1) break;

            EquipamentoEscudo* escudoParaConsertar = escudosDanificados[escolha];
            if (!NPCInteraction::verificarItemNaoEquipado(currentPlayer, escudoParaConsertar, "Bjorn", Color::CYAN, "Voce precisa DESEQUIPAR o escudo antes de conserta-lo!")) continue;

            int durabilidadePerdida = escudoParaConsertar->obterDurabilidadeMaxima() - escudoParaConsertar->obterDurabilidadeAtualEscudo();
            int custoReparo = durabilidadePerdida * 5; // Exemplo: 5 de ouro por ponto de durabilidade perdida

            if (currentPlayer->obterInventario()->obterOuro() >= custoReparo) {
                currentPlayer->obterInventario()->adicionarOuro(-custoReparo);
                escudoParaConsertar->definirDurabilidade(escudoParaConsertar->obterDurabilidadeMaxima());
                dialogoBjornUnico("Hmph! Seu escudo esta como novo! (-" + std::to_string(custoReparo) + "g)");
            } else {
                dialogoBjornUnico("Voce nao tem ouro suficiente para consertar este escudo. Eu preciso de " + std::to_string(custoReparo) + "g.");
            }
        } while (true);
    }
}
