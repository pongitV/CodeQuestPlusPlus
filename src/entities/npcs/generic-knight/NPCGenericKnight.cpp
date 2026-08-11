#include "NPCGenericKnight.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

#include "../../classes/warrior/Warrior.h"
#include "../../races/human/Human.h"
#include "../../../systems/inventory/ItemFactory.h"
#include "../../../systems/inventory/items/MaterialItem.h"
#include "../../../systems/combat/Combat.h"
#include "../../../core/state/EnemyCreator.h"
#include "../../../ui/screens/menu/ScreenMenu.h"
#include "../../../core/utils/RendererProvider.h"
#include "../../../core/utils/DialogFunctions.h"
#include "../../../systems/progress/Diary.h"
#include "../../../systems/progress/Progression.h"
#include "../../../systems/progress/ProgressionFlags.h"
#include "../../../core/utils/InputControl.h"
#include "../../../maps/control/MapController.h"
#include "NPCGenericKnightLayout.h"
#include "../../../core/utils/Color.h"

namespace {
    // --- CLASSES E FUNCOES AUXILIARES ---
    Item* buscarPorNome(Inventory* inv, const std::string& nome) {
        for (auto* item : inv->obterTodosOsItens()) {
            if (item->getNameItem() == nome) return item;
        }
        return nullptr;
    }

    class ClasseCavaleiro : public Warrior {
    public:
        std::string getNameClasse() const override { return "Cavaleiro Real"; }
        
        std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override {
            std::vector<std::unique_ptr<Item>> equipamentos;
            Warrior base;
            for (auto& item : base.obterEquipamentoClasse()) {
                if (item->obterTipo() == TipoEquipamento::ESCUDO || item->obterTipo() == TipoEquipamento::CONSUMIVEL) {
                    equipamentos.push_back(std::move(item));
                }
            }
            equipamentos.push_back(ItemFactory::criarItem(ItemID::EspadaCavaleiro));
            return equipamentos;
        }
    };

    class RacaCavaleiro : public Human {
    public:
        const std::vector<std::string>& getRaceAppearance() const override {
            return NPCGenericKnightLayouts::arteCavaleiro;
        }
    };

    // Comentario adicionado para strengthr a recompilacao e resolver erros de linkagem do Warrior
    // --- APARENCIA E DIALOGOS ---
    void dialogoCavaleiro(const std::vector<std::string>& linhas) {
        DialogFunctions::printNPCDialog("Cavaleiro Real", Color::GRAY, linhas);
    }

    bool buscarTrollProximo(const std::vector<std::string>& map, int startX, int startY, int& outX, int& outY) {
        for (int dy = -2; dy <= 2; ++dy) {
            for (int dx = -5; dx <= 5; ++dx) {
                int y = startY + dy;
                int x = startX + dx;
                if (y >= 0 && y < static_cast<int>(map.size()) && x >= 0 && x < static_cast<int>(map[y].size())) {
                    if (map[y][x] == 'T') {
                        outX = x;
                        outY = y;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void displayTelaCavaleiro(const std::string& tituloCabecalho, const std::vector<std::string>& falas) {
        if (RendererProvider::get()) RendererProvider::get()->displayPopup(tituloCabecalho, falas, Color::GRAY, NPCGenericKnightLayouts::arteCavaleiro);
    }
}

// --- CRIACAO DO NPC ---
std::unique_ptr<Character> NPCGenericKnight::criarCavaleiro(const std::string& nome) {
    auto cavaleiro = std::make_unique<Character>(nome, std::make_unique<RacaCavaleiro>(), std::make_unique<ClasseCavaleiro>());
    std::string nomeArmadura = ItemFactory::getNameDeID(ItemID::ArmaduraCavaleiro);
    std::string nomeEspada = ItemFactory::getNameDeID(ItemID::EspadaCavaleiro);
    cavaleiro->obterInventario()->adicionarItem(ItemFactory::criarItem(ItemID::ArmaduraCavaleiro));
    cavaleiro->equiparItem(buscarPorNome(cavaleiro->obterInventario(), nomeArmadura));
    cavaleiro->equiparItem(buscarPorNome(cavaleiro->obterInventario(), nomeEspada));
    cavaleiro->calcularAtributos();
    cavaleiro->modificarVida(cavaleiro->obterVidaMaxima());
    return cavaleiro;
}

// --- INTERACAO ---
void NPCGenericKnight::interagir(Character* currentPlayer, bool& trollDerrotado, bool& conviteRecebido, int /*larguraDoTerminal*/, std::vector<std::string>& matrizDoMapaAtual, bool exploracaoEstaAtiva, const std::function<void()>& restaurarTela, char celulaDestino, int proximaPosicaoX, int proximaPosicaoY) {
    Diary::instancia().registrarNPC("Cavaleiro Real");
    if (!trollDerrotado && (celulaDestino == 'T' || celulaDestino == 'C')) {
        int posicaoTrollX = -1, posicaoTrollY = -1;
        
        if (celulaDestino == 'T') {
            posicaoTrollX = proximaPosicaoX;
            posicaoTrollY = proximaPosicaoY;
        } else if (celulaDestino == 'C') {
            buscarTrollProximo(matrizDoMapaAtual, proximaPosicaoX, proximaPosicaoY, posicaoTrollX, posicaoTrollY);
        }

        if (posicaoTrollX == -1) {
            if (RendererProvider::get()) RendererProvider::get()->iniciarInteracaoPopup();
            std::vector<std::string> falas = {
                "Ainda temos invasores no reino!",
                "voce precisa de permissao se nao quiser ser",
                "tratado como invasor tambem...",
                "Nos ajude a derrotar todos e podemos",
                "garantir sua entrada no reino!"
            };
            if (RendererProvider::get()) RendererProvider::get()->displayPopup("CAVALEIROS REAIS", falas, Color::GRAY, NPCGenericKnightLayouts::arteCavaleiro);
            return;
        }

        if (RendererProvider::get()) RendererProvider::get()->iniciarInteracaoPopup();
        std::vector<std::string> falas = {
            "Viajante! Este Troll bloqueia a passagem.",
            "Nossas strengths estao se esgotando!",
            "Nos ajude a derrota-lo e o recompensaremos!"
        };
        
        int escolha = 1; // Default
        InputControl::executarLoopMenuPopup(
            [&]() { return std::vector<std::string>{""}; },
            [&]() { return std::vector<std::string>{"Ajudar os Cavaleiros", "Recuar"}; },
            [&](const std::string& op) {
                if (op == "Ajudar os Cavaleiros") escolha = 0;
                else escolha = 1;
                return false; // Exit loop after choice
            },
            "PEDIDO DE AJUDA", Color::GRAY, NPCGenericKnightLayouts::arteCavaleiro
        );

        if (escolha == 0) {
            Diary::instancia().registrarMissaoAceita("cavaleiro_trolls");
            std::vector<std::unique_ptr<Character>> aliados;
            aliados.push_back(criarCavaleiro("Cavaleiro Real 1"));
            aliados.push_back(criarCavaleiro("Cavaleiro Real 2"));
            
            std::vector<std::unique_ptr<Character>> enemies;
            auto trolls = EnemyCreator::createTrollEnemy(1);
            if (!trolls.empty()) enemies.push_back(std::move(trolls[0])); 
            
            Combat combat(currentPlayer, std::move(enemies));
            combat.adicionarAliados(std::move(aliados));
            if (MapControllera::isExploracao3DAtiva()) {
                combat.setContexto3D(
                    true, 
                    matrizDoMapaAtual, 
                    MapControllera::obterPosCamera3DX(), 
                    MapControllera::obterPosCamera3DY(), 
                    MapControllera::obterAnguloCamera3D(), 
                    MapControllera::obterTituloMapaAtual()
                );
            }
            combat.iniciarCombate();
            
            if (currentPlayer->obterVida() > 0) {
                matrizDoMapaAtual[posicaoTrollY][posicaoTrollX] = '.';
                
                int trollsRestantes = 0;
                for (const auto& linha : matrizDoMapaAtual) {
                    trollsRestantes += std::count(linha.begin(), linha.end(), 'T');
                }
                if (trollsRestantes == 0) {
                    trollDerrotado = true;
                    Progression::instancia().definirFlag(Flags::PonteReino_TrollDerrotado, true);
                }
            }
        }
        if (exploracaoEstaAtiva && !MapControllera::isExploracao3DAtiva()) restaurarTela();
    } else if (celulaDestino == 'C') {
        InputControl::executarLoopMenuPopup(
            [&]() {
                if (!conviteRecebido) {
                    return std::vector<std::string>{""};
                } else {
                    return std::vector<std::string>{""};
                }
            },
            [&]() { return std::vector<std::string>{"Conversar", "Missoes do Cavaleiro", "VOLTAR"}; },
            [&](const std::string& op) {
                if (op == "Conversar") {
                    if (!conviteRecebido) {
                    } else {
                    }
                    return true;
                } else if (op == "Missoes do Cavaleiro") {
                    if (!conviteRecebido) {
                        int escMissao = -1;
                        InputControl::executarLoopMenuPopup(
                            [&]() { return std::vector<std::string>{""}; },
                            [&]() { return std::vector<std::string>{"[M] Reportar Trolls derrotados", "VOLTAR"}; },
                            [&](const std::string& subOp) {
                                if (subOp == "[M] Reportar Trolls derrotados") { escMissao = 0; return false; }
                                return false;
                            },
                            "MISSOES - CAVALEIRO", Color::GRAY, NPCGenericKnightLayouts::arteCavaleiro
                        );

                        if (escMissao == 0) {
                            std::vector<std::string> recompensaFalas = {
                                "Voce lutou bravamente e limpou o reino dos Trolls!",
                                "Como prometido, aqui esta a sua recompensa.",
                                "",
                                "Voce recebeu o [Convite Real]!"
                            };
                            currentPlayer->obterInventario()->adicionarItem(ItemFactory::criarItem(ItemID::ConviteReal));
                            Diary::instancia().registrarItem("Convite Real");
                            Diary::instancia().registrarMissaoConcluida("cavaleiro_trolls");
                            Progression::instancia().definirFlag(Flags::Vila_ConviteReal, true);
                            conviteRecebido = true;
                        }
                    } else {
                        InputControl::executarLoopMenuPopup(
                            [&]() { return std::vector<std::string>{""}; },
                            [&]() { return std::vector<std::string>{"(Nenhuma missao disponivel)", "VOLTAR"}; },
                            [&](const std::string& subOp) {
                                if (subOp == "(Nenhuma missao disponivel)") {
                                }
                                return false;
                            },
                            "MISSOES - CAVALEIRO", Color::GRAY, NPCGenericKnightLayouts::arteCavaleiro
                        );
                    }
                    return true;
                } else if (op == "VOLTAR" || op == "Sair") {
                    return false;
                }
                return true;
            },
            "CAVALEIRO REAL", Color::GRAY, NPCGenericKnightLayouts::arteCavaleiro
        );
        if (exploracaoEstaAtiva && !MapControllera::isExploracao3DAtiva()) restaurarTela();
    }
}
