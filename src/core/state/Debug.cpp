#include "Debug.h"
#include <windows.h>
#include "../../entities/character/Character.h"
#include "../utils/InputControl.h"
#include "../utils/StringBuffer.h"
#include "../d2d-context/D2DContext.h"
#include "../../rendering/direct-2d/D2DRenderer.h"
#include "../../rendering/direct-2d/UIRenderer2D.h"
#include "../window/GameWindow.h"
#include "../../systems/progress/Diary.h"
#include "../../systems/progress/Bestiary.h"
#include "../../systems/progress/Progression.h"
#include "../../systems/progress/ProgressionFlags.h"
#include "../../rendering/raycaster/screens/utils/MenuRaycasterUtils.h"
#include "../state/EnemyCreator.h"
#include "../../systems/combat/Combat.h"
#include "../../systems/combat/CombatRaycasterUIImpl.h"
#include "../../maps/control/MapController.h"
#include "../../ui/UIManager.h"
#include <vector>
#include <string>
#include <thread>
#include <chrono>

std::atomic<bool> Debug::isGodModeActive{false};
std::atomic<bool> Debug::isNoclipActive{false};
std::atomic<bool> Debug::isOneHitKillActive{false};
std::atomic<bool> Debug::isSpeedHackActive{false};

bool Debug::isDebugKey(char tecla) {
    if (tecla == '=' || tecla == '\\' || tecla == '`') return true;
    auto isPressed = [](int key) { return (GetAsyncKeyState(key) & 0x8000) != 0; };
    return isPressed(VK_F12) || isPressed(VK_OEM_PLUS) || isPressed(0xBB);
}

void Debug::displayDebugMenu(Character* jogador) {
    ClipCursor(nullptr);
    ShowCursor(TRUE);
    InputControl::clearBuffer();

    std::string mensagemFeedback = "";
    bool inMenu = true;

    while (inMenu) {
        int ouro = (jogador && jogador->obterInventario()) ? jogador->obterInventario()->obterOuro() : 0;
        int nivel = jogador ? jogador->getLevel() : 1;
        int hp = jogador ? jogador->obterVida() : 0;
        int hpMax = jogador ? jogador->obterVidaMaxima() : 0;
        int str = jogador ? jogador->getStrength() : 0;
        int dex = jogador ? jogador->getDexterity() : 0;
        int res = jogador ? jogador->getResistance() : 0;
        int con = jogador ? jogador->getConstitution() : 0;
        int intel = jogador ? jogador->getInteligencia() : 0;
        int wis = jogador ? jogador->getWisdom() : 0;
        int xp = jogador ? jogador->getXpAtual() : 0;

        std::vector<std::string> opcoes = {
            "GOD MODE: " + std::string(isGodModeActive ? "[ATIVADO]" : "[DESATIVADO]"),
            "NOCLIP (3D): " + std::string(isNoclipActive ? "[ATIVADO]" : "[DESATIVADO]"),
            "ONE-HIT KILL: " + std::string(isOneHitKillActive ? "[ATIVADO]" : "[DESATIVADO]"),
            "SUPER VELOCIDADE: " + std::string(isSpeedHackActive ? "[ATIVADO]" : "[DESATIVADO]"),
            "DEFINIR OURO (Atual: " + std::to_string(ouro) + " G)",
            "DEFINIR NIVEL (Atual: Lv." + std::to_string(nivel) + ")",
            "DEFINIR VIDA MAXIMA (Atual: " + std::to_string(hp) + "/" + std::to_string(hpMax) + " HP)",
            "DEFINIR FORCA (STR: " + std::to_string(str) + ")",
            "DEFINIR DESTREZA (DEX: " + std::to_string(dex) + ")",
            "DEFINIR RESISTENCIA (RES: " + std::to_string(res) + ")",
            "DEFINIR CONSTITUICAO (CON: " + std::to_string(con) + ")",
            "DEFINIR INTELIGENCIA (INT: " + std::to_string(intel) + ")",
            "DEFINIR SABEDORIA (WIS: " + std::to_string(wis) + ")",
            "DEFINIR EXPERIENCIA (XP: " + std::to_string(xp) + ")",
            "RESTAURAR VIDA COMPLETA (100% HP)",
            "DESBLOQUEAR TODOS NPCS NO DIARIO",
            "DESBLOQUEAR BESTIARIO COMPLETO",
            "DESBLOQUEAR VIAGEM RAPIDA",
            "INICIAR COMBATE PERSONALIZADO (CHEAT)",
            "VOLTAR AO JOGO"
        };

        auto construtorCaixa = [&](UIDynamicBox& box, int selecaoAtual, float logicalW, float logicalH) {
            box.SetTitle(L"[ PAINEL DE CHEATS / DEBUG ]", D2D1::ColorF(1.0f, 0.84f, 0.0f));

            float startY = 75.0f;
            float stepY = 25.0f;

            for (int i = 0; i < (int)opcoes.size(); ++i) {
                std::wstring wOpc = MenuRaycasterUtils::utf8_to_wstring(opcoes[i]);
                D2D1_COLOR_F cor = (i == selecaoAtual) ? D2D1::ColorF(0.0f, 1.0f, 0.5f) : D2D1::ColorF(0.85f, 0.85f, 0.85f);
                MenuRaycasterUtils::adicionarOpcaoMenu(box, wOpc, logicalW / 2.0f, startY + i * stepY, (i == selecaoAtual), cor, true);
            }

            if (!mensagemFeedback.empty()) {
                std::wstring wFeed = MenuRaycasterUtils::utf8_to_wstring(mensagemFeedback);
                box.AddText(wFeed, logicalW / 2.0f, startY + opcoes.size() * stepY + 12.0f, 16.0f, D2D1::ColorF(0.0f, 0.95f, 1.0f), true);
            }

            box.AddText(L"[ENTER / ESPACO] Alterar / Digitar Valor  |  [ESC] Sair", logicalW / 2.0f, startY + opcoes.size() * stepY + 40.0f, 14.0f, D2D1::ColorF(0.6f, 0.6f, 0.6f), true);
        };

        int escolha = MenuRaycasterUtils::renderizarPopupCaixa({}, {}, (int)opcoes.size(), construtorCaixa);

        if (escolha == -1 || escolha == 19) {
            inMenu = false;
        } else if (escolha == 0) {
            isGodModeActive = !isGodModeActive.load();
            if (jogador && isGodModeActive) {
                jogador->obterAtributosFinais().health += 99999;
                jogador->strengthrRecalculoCache();
                jogador->definirVida(jogador->obterVidaMaxima());
            }
            mensagemFeedback = isGodModeActive ? "[!] GODMODE ATIVADO!" : "[!] GODMODE DESATIVADO!";
        } else if (escolha == 1) {
            isNoclipActive = !isNoclipActive.load();
            mensagemFeedback = isNoclipActive ? "[!] NOCLIP ATIVADO!" : "[!] NOCLIP DESATIVADO!";
        } else if (escolha == 2) {
            isOneHitKillActive = !isOneHitKillActive.load();
            mensagemFeedback = isOneHitKillActive ? "[!] ONE-HIT KILL ATIVADO!" : "[!] ONE-HIT KILL DESATIVADO!";
        } else if (escolha == 3) {
            isSpeedHackActive = !isSpeedHackActive.load();
            mensagemFeedback = isSpeedHackActive ? "[!] SUPER VELOCIDADE ATIVADA!" : "[!] SUPER VELOCIDADE DESATIVADA!";
        } else if (escolha == 4) { // OURO
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite a quantidade de OURO desejada:", 9);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val < 0) val = 0;
                    if (jogador && jogador->obterInventario()) {
                        int atualVal = jogador->obterInventario()->obterOuro();
                        jogador->ganharOuro(val - atualVal);
                        mensagemFeedback = "[!] Ouro alterado para " + std::to_string(val) + " G!";
                    }
                } catch (...) {}
            }
        } else if (escolha == 5) { // NIVEL
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite o NIVEL desejado (1 - 100):", 4);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val >= 1 && val <= 100 && jogador) {
                        jogador->definirNivel(val);
                        mensagemFeedback = "[!] Nível alterado para " + std::to_string(val) + "!";
                    }
                } catch (...) {}
            }
        } else if (escolha == 6) { // VIDA MAXIMA
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite a VIDA MAXIMA desejada:", 7);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val >= 1 && jogador) {
                        jogador->obterAtributosFinais().health = val;
                        jogador->strengthrRecalculoCache();
                        jogador->definirVida(val);
                        mensagemFeedback = "[!] Vida Máxima definida para " + std::to_string(val) + " HP!";
                    }
                } catch (...) {}
            }
        } else if (escolha == 7) { // FORCA
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite o valor de FORCA:", 5);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val >= 1 && jogador) {
                        jogador->obterAtributosFinais().strength = val;
                        jogador->strengthrRecalculoCache();
                        mensagemFeedback = "[!] Força alterada para " + std::to_string(val) + "!";
                    }
                } catch (...) {}
            }
        } else if (escolha == 8) { // DESTREZA
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite o valor de DESTREZA:", 5);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val >= 1 && jogador) {
                        jogador->obterAtributosFinais().dexterity = val;
                        jogador->strengthrRecalculoCache();
                        mensagemFeedback = "[!] Destreza alterada para " + std::to_string(val) + "!";
                    }
                } catch (...) {}
            }
        } else if (escolha == 9) { // RESISTENCIA
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite o valor de RESISTENCIA:", 5);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val >= 1 && jogador) {
                        jogador->obterAtributosFinais().resistance = val;
                        jogador->strengthrRecalculoCache();
                        mensagemFeedback = "[!] Resistência alterada para " + std::to_string(val) + "!";
                    }
                } catch (...) {}
            }
        } else if (escolha == 10) { // CONSTITUICAO
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite o valor de CONSTITUICAO:", 5);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val >= 1 && jogador) {
                        jogador->obterAtributosFinais().constitution = val;
                        jogador->strengthrRecalculoCache();
                        mensagemFeedback = "[!] Constituição alterada para " + std::to_string(val) + "!";
                    }
                } catch (...) {}
            }
        } else if (escolha == 11) { // INTELIGENCIA
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite o valor de INTELIGENCIA:", 5);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val >= 1 && jogador) {
                        jogador->obterAtributosFinais().intelligence = val;
                        jogador->strengthrRecalculoCache();
                        mensagemFeedback = "[!] Inteligência alterada para " + std::to_string(val) + "!";
                    }
                } catch (...) {}
            }
        } else if (escolha == 12) { // SABEDORIA
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite o valor de SABEDORIA:", 5);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val >= 1 && jogador) {
                        jogador->obterAtributosFinais().wisdom = val;
                        jogador->strengthrRecalculoCache();
                        mensagemFeedback = "[!] Sabedoria alterada para " + std::to_string(val) + "!";
                    }
                } catch (...) {}
            }
        } else if (escolha == 13) { // XP
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite a quantidade de XP:", 7);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val >= 0 && jogador) {
                        jogador->definirXpAtual(val);
                        mensagemFeedback = "[!] XP alterado para " + std::to_string(val) + "!";
                    }
                } catch (...) {}
            }
        } else if (escolha == 14) { // RESTAURAR VIDA
            if (jogador) jogador->definirVida(jogador->obterVidaMaxima());
            mensagemFeedback = "[!] Vida restaurada para 100% HP!";
        } else if (escolha == 15) { // REGISTRAR NPCS
            Diary::instance().registerNPC("Bjorn (Blacksmith)");
            Diary::instance().registerNPC("Franchesco (Merchant)");
            Diary::instance().registerNPC("Morgana (MageNPC)");
            Diary::instance().registerNPC("Anok (Estilista)");
            Diary::instance().registerNPC("Priest Benedito");
            mensagemFeedback = "[!] Todos os NPCs foram registrados no Diário!";
        } else if (escolha == 16) { // BESTIARIO
            for (const auto& nome : Bestiary::instance().getEnemiesOrderedByDifficulty()) {
                Bestiary::instance().registerFirstSight(nome);
                Bestiary::instance().registerDefeat(nome);
            }
            mensagemFeedback = "[!] Todos os inimigos foram desbloqueados no Bestiário!";
        } else if (escolha == 17) { // VIAGEM RAPIDA
            Progression::instance().setFlag(Flags::Visited_Forest, true);
            Progression::instance().setFlag(Flags::Visited_KingdomBridge, true);
            Progression::instance().setFlag(Flags::Visited_Kingdom, true);
            Progression::instance().setFlag(Flags::Discovered_Maps, true);
            mensagemFeedback = "[!] Viagem rápida desbloqueada para todos os mapas!";
        } else if (escolha == 18) { // INICIAR COMBATE PERSONALIZADO
            std::vector<std::string> especies = {
                "Goblin",
                "Slime",
                "Fada",
                "Ork Exilado",
                "Abominação da Floresta",
                "Troll",
                "Mímico",
                "Mahoraga",
                "CANCELAR"
            };

            int escolhaEspecie = MenuRaycasterUtils::renderizarPopupCaixa({}, {}, (int)especies.size(), [&](UIDynamicBox& box, int sel, float w, float h) {
                box.SetTitle(L"[ SELECIONE O INIMIGO PARA COMBATE ]", D2D1::ColorF(1.0f, 0.84f, 0.0f));
                float startY = 110.0f;
                for (int i = 0; i < (int)especies.size(); ++i) {
                    std::wstring text = MenuRaycasterUtils::utf8_to_wstring(especies[i]);
                    D2D1_COLOR_F color = (i == sel) ? D2D1::ColorF(0.0f, 1.0f, 0.5f) : D2D1::ColorF(0.85f, 0.85f, 0.85f);
                    MenuRaycasterUtils::adicionarOpcaoMenu(box, text, w / 2.0f, startY + i * 32.0f, (i == sel), color, true);
                }
            });

            if (escolhaEspecie >= 0 && escolhaEspecie < 8) {
                std::string strQtd = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite a quantidade de inimigos (1 - 5):", 2);
                int qtd = 1;
                if (!strQtd.empty()) {
                    try {
                        qtd = std::stoi(strQtd);
                        if (qtd < 1) qtd = 1;
                        if (qtd > 5) qtd = 5;
                    } catch (...) {}
                }

                std::vector<std::unique_ptr<Character>> inimigos;
                switch (escolhaEspecie) {
                    case 0: inimigos = EnemyCreator::createGoblinEnemy(qtd); break;
                    case 1: inimigos = EnemyCreator::createSlimeEnemy(qtd); break;
                    case 2: inimigos = EnemyCreator::createFairyEnemy(qtd); break;
                    case 3: inimigos = EnemyCreator::createExiledOrcEnemy(qtd); break;
                    case 4: inimigos = EnemyCreator::createForestAbominationEnemy(qtd); break;
                    case 5: inimigos = EnemyCreator::createTrollEnemy(qtd); break;
                    case 6: inimigos = EnemyCreator::createMimicEnemy(qtd); break;
                    case 7: inimigos = EnemyCreator::createMahoragaEnemy(qtd); break;
                }

                if (!inimigos.empty() && jogador) {
                    std::unique_ptr<ICombateUI> ui = nullptr;
                    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
                        ui = std::make_unique<CombateRaycasterUIImpl>();
                    }
                    Combat combat(jogador, std::move(inimigos), std::move(ui));
                    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
                        combat.setContexto3D(true, MapControllera::obterMatrizDoMapaAtual(),
                                             MapControllera::obterPosCamera3DX(),
                                             MapControllera::obterPosCamera3DY(),
                                             MapControllera::obterAnguloCamera3D(),
                                             MapControllera::obterTituloMapaAtual());
                    }
                    combat.iniciarCombate();
                    mensagemFeedback = "[!] Combate finalizado!";
                }
            }
        }
    }
    InputControl::clearBuffer();
}
