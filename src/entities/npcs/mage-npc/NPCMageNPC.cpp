#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <map>

#include "NPCMageNPC.h"
#include "../../../ui/screens/menu/ScreenMenu.h"
#include "../../../systems/inventory/Item.h"
#include "../../../systems/inventory/ItemFactory.h"
#include "../../../ui/screens/inventory/ScreenInventory.h"
#include "../../../core/utils/InputControl.h"
#include "../../../systems/inventory/equipment/WeaponEquipment.h"
#include "../../../core/state/Store.h"
#include "../../../core/utils/DialogFunctions.h"
#include "../../../systems/progress/Diary.h"
#include "../../../systems/progress/Progression.h"
#include "../../../systems/progress/ProgressionFlags.h"
#include "../../../ui/screens/ScreenBase.h"
#include "NPCMageNPCLayout.h"
#include "../../../core/utils/Color.h"

namespace {
    std::map<int, StoreProduct> estoquePocoesBuff = {
        {1, {ItemID::PocaoFuria, 25, -1}},
        {2, {ItemID::ElixirArcano, 25, -1}}
    };

    std::map<int, StoreProduct> estoquePocoesDebuff = {
        {1, {ItemID::FrascoGosma, 30, -1}},
        {2, {ItemID::FrascoFraqueza, 30, -1}}
    };

    struct EncantoOperacao {
        std::string nomeMenu;
        ItemID materialId;
        int qtd;
        ItemID armaRestritaId; 
        std::function<bool(EquipamentoArma*)> checarConflito;
        std::string msgConflito;
        std::function<std::string(Character*, EquipamentoArma*)> aplicar;
    };

    const std::vector<EncantoOperacao> operacoesDeEncantamento = {
        { "Sangramento (40x Dente de Goblin)", ItemID::DenteGoblin, 40, ItemID::Nenhum, 
          [](EquipamentoArma* a){ return a->possuiEfeitoSangramento(); }, "Esta arma ja esta encantada com Sangramento!",
          [](Character*, EquipamentoArma* a){ a->aplicarEfeitoSangramento(); a->alterarNome(a->getNameItem() + " (Sangrenta)"); return a->getNameItem(); } },
          
        { "Lentidao (5x Nucleo pegajoso)", ItemID::NucleoPegajoso, 5, ItemID::Nenhum,
          [](EquipamentoArma* a){ return a->possuiEfeitoLentidao(); }, "Esta arma ja esta encantada com Lentidao!",
          [](Character*, EquipamentoArma* a){ a->aplicarEfeitoLentidao(); a->alterarNome(a->getNameItem() + " (Viscosa)"); return a->getNameItem(); } },
          
        { "Quebra de Resistencia (25x Po magico)", ItemID::PoMagico, 25, ItemID::Nenhum,
          [](EquipamentoArma* a){ return a->temPropriedade(Propriedade::Penetrante); }, "Esta arma ja esta encantada com Reducao de Resistencia!",
          [](Character*, EquipamentoArma* a){ a->alterarNome(a->getNameItem() + " (Quebra-Defesas)"); a->adicionarPropriedade(Propriedade::Penetrante); return a->getNameItem(); } },
          
        { "Arco recurvo de madeira: Magia (1x Madeira enfeiticada)", ItemID::MadeiraEnfeiticada, 1, ItemID::ArcoMadeira,
          [](EquipamentoArma* a){ return a->temPropriedade(Propriedade::Magica); }, "Esta arma ja esta encantada com Magia!",
          [](Character* currentPlayer, EquipamentoArma* armaEscolhida) {
              std::string nomeArco = ItemFactory::getNameDeID(ItemID::ArcoMadeira);
              std::string nome = armaEscolhida->getNameItem();
              size_t pos = nome.find(nomeArco);
              if (pos != std::string::npos) nome.replace(pos, 23, "Arco recurvo de madeira enfeiticada");
              int novoDanoMagico = armaEscolhida->obterDanoMagico() + (armaEscolhida->obterDanoFisico() / 2);
              auto novoArcoObj = std::make_unique<EquipamentoArma>(nome, armaEscolhida->obterDanoFisico(), novoDanoMagico, armaEscolhida->obterReqForca(), armaEscolhida->obterReqDestreza(), armaEscolhida->obterReqInteligencia(), armaEscolhida->obterReqSabedoria(), 0);
              EquipamentoArma* novoArco = novoArcoObj.get();
              if (armaEscolhida->possuiEfeitoSangramento()) novoArco->aplicarEfeitoSangramento();
              if (armaEscolhida->possuiEfeitoLentidao()) novoArco->aplicarEfeitoLentidao();
              if (armaEscolhida->temPropriedade(Propriedade::Penetrante)) novoArco->adicionarPropriedade(Propriedade::Penetrante);
              novoArco->adicionarPropriedade(Propriedade::Magica);

              bool estavaEquipado = (currentPlayer->obterArma() == armaEscolhida);
              if (estavaEquipado) currentPlayer->desequiparArma();
              currentPlayer->obterInventario()->removerItem(armaEscolhida);
              currentPlayer->obterInventario()->adicionarItem(std::move(novoArcoObj));
              if (estavaEquipado) currentPlayer->equiparItem(novoArco);
              return novoArco->getNameItem();
          } },
          
        { "Cajado de cristal magico: Cipos (1x Coracao da floresta)", ItemID::CoracaoFloresta, 1, ItemID::CajadoCristal,
          [](EquipamentoArma* a){ return a->temPropriedade(Propriedade::CipoPrisao); }, "Esta arma ja esta encantada com Cipos!",
          [](Character*, EquipamentoArma* a){
              std::string nomeCajado = ItemFactory::getNameDeID(ItemID::CajadoCristal);
              std::string nome = a->getNameItem();
              size_t pos = nome.find(nomeCajado);
              if (pos != std::string::npos) nome.replace(pos, 24, "Cajado de cipos");
              a->alterarNome(nome);
              a->adicionarPropriedade(Propriedade::CipoPrisao);
              return a->getNameItem();
          } },
          
        { "Violao encantado: Raizes (1x Madeira enfeiticada)", ItemID::MadeiraEnfeiticada, 1, ItemID::ViolaoEncantado,
          [](EquipamentoArma* a){ return a->temPropriedade(Propriedade::ViolaoMagico); }, "Esta arma ja esta encantada com Raizes!",
          [](Character*, EquipamentoArma* a){
              std::string nomeViolao = ItemFactory::getNameDeID(ItemID::ViolaoEncantado);
              std::string nome = a->getNameItem();
              size_t pos = nome.find(nomeViolao);
              if (pos != std::string::npos) nome.replace(pos, 16, "Violao enfeiticado");
              else nome += " enfeiticado";
              a->alterarNome(nome);
              a->adicionarPropriedade(Propriedade::ViolaoMagico);
              return a->getNameItem();
          } }
    };

    // --- APARENCIA E DIALOGOS ---
    void processarEncantamentos(Character* currentPlayer, bool isUniversal);
    void processarPocoes(Character* currentPlayer, bool isBuff);
    void processarMissaoLabirinto(Character* currentPlayer);
    void processarMenuMissoes(Character* currentPlayer);

    void dialogoMorgana(const std::vector<std::string>& linhas) {
    }
    
    void dialogoMorganaUnico(const std::string& msg) {
        InputControl::lerSelecaoMenuEmPopup("Morgana", {msg}, {"OK"}, Color::CYAN, NPCMageNPCLayouts::arteMageNPC);
    }
}

// --- INFORMACOES DO LUGAR ---
std::string NPCMageNPC::getNameDoLugar() const {
    return "CABANA DA BRUXA";
}

Color NPCMageNPC::obterCorDoCabecalho() const {
    return Color::MAGENTA;
}

Color NPCMageNPC::obterCorDaArte() const {
    return Color::MAGENTA;
}

const std::vector<std::string>& NPCMageNPC::obterArteASCII() const {
    return NPCMageNPCLayouts::arteMageNPC;
}

// --- INTERACAO E MENU ---
void NPCMageNPC::interagir(Character* jogador) {
    InputControl::executarLoopMenuPopup(
        [this, jogador]() { return this->obterDialogo(jogador); },
        [this, jogador]() { return this->obterOpcoesMenu(jogador, 120); },
        [this, jogador](const std::string& op) { this->processarOpcao(jogador, op, 120); return true; },
        getNameDoLugar(), obterCorDoCabecalho(), obterArteASCII()
    );
}

std::vector<std::string> NPCMageNPC::obterDialogo(Character* /*jogador*/) {
    if (Progression::instancia().obterFlag(Flags::Floresta_MissaoMorgana)) {
        return std::vector<std::string>{
            "O Labirinto o aguarda..."
        };
    } else {
        return std::vector<std::string>{
            "Hmmm... sinto cheiro de poder no ar.",
            "O que voce busca, viajante?"
        };
    }
}

std::vector<std::string> NPCMageNPC::obterOpcoesMenu(Character* /*jogador*/, int /*larguraDoTerminal*/) {
    return {
        "ENCANTAR Armas (Universais)",
        "ENCANTAR Armas (Especificas)",
        "COMPRAR Pocoes de Buff",
        "COMPRAR Frascos de Debuff",
        "Missoes de Morgana",
        "VOLTAR"
    };
}

void NPCMageNPC::processarOpcao(Character* jogador, const std::string& opcao, int /*larguraDoTerminal*/) {
    if (opcao == "ENCANTAR Armas (Universais)") {
        processarEncantamentos(jogador, true);
    }
    else if (opcao == "ENCANTAR Armas (Especificas)") {
        processarEncantamentos(jogador, false);
    }
    else if (opcao == "COMPRAR Pocoes de Buff" || opcao == "COMPRAR Frascos de Debuff") {
        processarPocoes(jogador, opcao == "COMPRAR Pocoes de Buff");
    }
    else if (opcao == "Missoes de Morgana") {
        processarMenuMissoes(jogador);
    }
}

namespace {
    // --- PROCESSAMENTO DE OPCOES ---
    void processarEncantamentos(Character* currentPlayer, bool isUniversal) {
        std::vector<const EncantoOperacao*> opsAtuais;
        int inicio = isUniversal ? 0 : 3;
        int fim = isUniversal ? 3 : 6;
        for (int i = inicio; i < fim; ++i) {
            opsAtuais.push_back(&operacoesDeEncantamento[i]);
        }


        while (true) {
            std::vector<std::string> linhas;
            for (auto* op : opsAtuais) linhas.push_back(op->nomeMenu);
            linhas.push_back("VOLTAR");

            int id = InputControl::lerSelecaoMenuEmPopup(
                isUniversal ? "CABANA - ENCANTOS UNIVERSAIS" : "CABANA - ENCANTOS ESPECIFICOS",
                {"Escolha um encantamento:"},
                linhas,
                Color::MAGENTA,
                NPCMageNPCLayouts::arteMageNPC
            );

            if (id == static_cast<int>(opsAtuais.size()) || id == -1) {
                break;
            }

            const auto& op = *opsAtuais[id];
            
            std::string itemNecessario = ItemFactory::getNameDeID(op.materialId);
            int qtdAtual = currentPlayer->obterInventario()->contarItem(itemNecessario);
            if (qtdAtual < op.qtd) {
                dialogoMorganaUnico("Voce nao tem " + itemNecessario + " suficiente! (Possui: " + std::to_string(qtdAtual) + "/" + std::to_string(op.qtd) + ")");
                continue;
            }
            
            std::vector<Item*> itensValidos;
            std::vector<std::string> opcoesItem;
            for (auto* item : currentPlayer->obterInventario()->obterTodosOsItens()) {
                if (item->obterTipo() == TipoEquipamento::ARMA) {
                    itensValidos.push_back(item);
                    opcoesItem.push_back(item->getNameItem());
                }
            }
            if (opcoesItem.empty()) { dialogoMorganaUnico("Voce nao tem armas para encantar!"); continue; }
            opcoesItem.push_back("VOLTAR");
            
            int escolhaArma = InputControl::lerSelecaoMenuEmPopup("ESCOLHA UMA ARMA", {"Qual arma deseja encantar?"}, opcoesItem, Color::MAGENTA, NPCMageNPCLayouts::arteCaldeirao);
            if (escolhaArma == -1 || escolhaArma == static_cast<int>(opcoesItem.size()) - 1) continue;
            
            EquipamentoArma* armaEscolhida = dynamic_cast<EquipamentoArma*>(itensValidos[escolhaArma]);
            
            if (op.armaRestritaId != ItemID::Nenhum) {
                std::string nomeRestrito = ItemFactory::getNameDeID(op.armaRestritaId);
                if (armaEscolhida->getNameItem().find(nomeRestrito) == std::string::npos) {
                    dialogoMorganaUnico("Este encantamento so funciona no " + nomeRestrito + "!");
                    continue;
                }
            }
            
            if (op.checarConflito(armaEscolhida)) {
                dialogoMorganaUnico(op.msgConflito);
                continue;
            }
            
            std::string nomeAntigoArma = armaEscolhida->getNameItem();
            for (int i = 0; i < op.qtd; ++i) currentPlayer->obterInventario()->removerItem(itemNecessario);
            
            std::string novoNome = op.aplicar(currentPlayer, armaEscolhida);
            
            std::string equacao = "[" + nomeAntigoArma + "] + " + std::to_string(op.qtd) + "x [" + itemNecessario + "] = [" + novoNome + "]";
        }
    }

    void processarPocoes(Character* currentPlayer, bool isBuff) {
        std::string titulo = isBuff ? "CABANA - POCOES DE BUFF" : "CABANA - FRASCOS DE DEBUFF";
        auto& estoqueAtual = isBuff ? estoquePocoesBuff : estoquePocoesDebuff;
        
        Store::processPurchase(currentPlayer, titulo, Color::MAGENTA, estoqueAtual, 
            [](const std::string& msg) { dialogoMorganaUnico(msg); }, NPCInteraction::obterFormatadorStatusItem, NPCMageNPCLayouts::arteMageNPC);
    }

    void processarMissaoLabirinto(Character* currentPlayer) {
        std::string nomeCoracao = ItemFactory::getNameDeID(ItemID::CoracaoFloresta);
        int qtdCoracoes = currentPlayer->obterInventario()->contarItem(nomeCoracao);

        if (qtdCoracoes < 3) {
            dialogoMorgana({
                "Voce ainda nao possui os 3 Coracoes da floresta que eu pedi. (Possui: " + std::to_string(qtdCoracoes) + "/3)",
                "Eles sao dropados por Abominacoes no Coracao da Arvore."
            });
            return;
        }

        for (int i = 0; i < 3; ++i) currentPlayer->obterInventario()->removerItem(nomeCoracao);
        currentPlayer->desbloquearLabirinto();
        Diary::instancia().registrarMissaoConcluida("morgana_coracoes");
        Progression::instancia().definirFlag(Flags::Floresta_MissaoMorgana, true);
        
        std::vector<std::string> dialogo = {
            "Ah, perfeitos! Estes coracoes pulsam com uma magia ancestral.",
            "Como recompensa, revelarei um segredo... Atras de mim, ha uma passagem secreta.",
            "Use a entrada [^L] para explorar o meu Labirinto Subterraneo.",
            "E um lugar perigoso, mergulhado em uma nevoa de cor roxa, mas guarda grandes tesouros."
        };
    }

    void processarMenuMissoes(Character* currentPlayer) {
        Diary::instancia().registrarMissaoAceita("morgana_coracoes");
        while (true) {
            std::vector<std::string> missoes;
            if (!currentPlayer->obterLabirintoDesbloqueado()) {
                std::string nomeCoracao = ItemFactory::getNameDeID(ItemID::CoracaoFloresta);
                int qtdCoracoes = currentPlayer->obterInventario()->contarItem(nomeCoracao);
                if (qtdCoracoes >= 3) {
                    missoes.push_back("[M] Entregar 3x Coracoes da floresta (Pronta)");
                } else {
                    missoes.push_back("[M] Consiga 3x Coracoes da floresta");
                }
            } else {
                missoes.push_back("(Nenhuma missao disponivel)");
            }
            missoes.push_back("VOLTAR");

            int id = InputControl::lerSelecaoMenuEmPopup(
                "MISSOES DE MORGANA",
                {"Escolha uma missao:"},
                missoes,
                Color::MAGENTA,
                NPCMageNPCLayouts::arteMageNPC
            );

            if (!currentPlayer->obterLabirintoDesbloqueado() && id == 0) {
                processarMissaoLabirinto(currentPlayer);
            } else if (currentPlayer->obterLabirintoDesbloqueado() && id == 0) {
                dialogoMorganaUnico("Nao busco mais nada de voce no momento...");
            } else if (id == 1 || id == -1) {
                break;
            }
        }
    }
}
