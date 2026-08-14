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

bool Debug::isDebugKey(char key) {
    if (key == '=' || key == '\\' || key == '`') return true;
    auto isPressed = [](int k) { return (GetAsyncKeyState(k) & 0x8000) != 0; };
    return isPressed(VK_F12) || isPressed(VK_OEM_PLUS) || isPressed(0xBB);
}

void Debug::showDebugMenu(Character* player) {
    ClipCursor(nullptr);
    GameWindow::showCursor();
    InputControl::clearBuffer();

    std::string feedbackMessage = "";
    bool inMenu = true;

    while (inMenu) {
        int gold = (player && player->obterInventario()) ? player->obterInventario()->obterOuro() : 0;
        int level = player ? player->getLevel() : 1;
        int hp = player ? player->obterVida() : 0;
        int maxHp = player ? player->obterVidaMaxima() : 0;
        int str = player ? player->getStrength() : 0;
        int dex = player ? player->getDexterity() : 0;
        int res = player ? player->getResistance() : 0;
        int con = player ? player->getConstitution() : 0;
        int intel = player ? player->getInteligencia() : 0;
        int wis = player ? player->getWisdom() : 0;
        int xp = player ? player->getXpAtual() : 0;

        std::vector<std::string> options = {
            "GOD MODE: " + std::string(isGodModeActive ? "[ATIVADO]" : "[DESATIVADO]"),
            "NOCLIP (3D): " + std::string(isNoclipActive ? "[ATIVADO]" : "[DESATIVADO]"),
            "ONE-HIT KILL: " + std::string(isOneHitKillActive ? "[ATIVADO]" : "[DESATIVADO]"),
            "SUPER VELOCIDADE: " + std::string(isSpeedHackActive ? "[ATIVADO]" : "[DESATIVADO]"),
            "DEFINIR OURO (Atual: " + std::to_string(gold) + " G)",
            "DEFINIR NIVEL (Atual: Lv." + std::to_string(level) + ")",
            "DEFINIR VIDA MAXIMA (Atual: " + std::to_string(hp) + "/" + std::to_string(maxHp) + " HP)",
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

        auto boxBuilder = [&](UIDynamicBox& box, int currentSelection, float logicalW, float logicalH) {
            box.SetTitle(L"[ PAINEL DE CHEATS / DEBUG ]", D2D1::ColorF(1.0f, 0.84f, 0.0f));

            float startY = 75.0f;
            float stepY = 25.0f;

            for (int i = 0; i < (int)options.size(); ++i) {
                std::wstring wOpc = MenuRaycasterUtils::utf8_to_wstring(options[i]);
                D2D1_COLOR_F cor = (i == currentSelection) ? D2D1::ColorF(0.0f, 1.0f, 0.5f) : D2D1::ColorF(0.85f, 0.85f, 0.85f);
                MenuRaycasterUtils::adicionarOpcaoMenu(box, wOpc, logicalW / 2.0f, startY + i * stepY, (i == currentSelection), cor, true);
            }

            if (!feedbackMessage.empty()) {
                std::wstring wFeed = MenuRaycasterUtils::utf8_to_wstring(feedbackMessage);
                box.AddText(wFeed, logicalW / 2.0f, startY + options.size() * stepY + 12.0f, 16.0f, D2D1::ColorF(0.0f, 0.95f, 1.0f), true);
            }

            box.AddText(L"[ENTER / ESPACO] Alterar / Digitar Valor  |  [ESC] Sair", logicalW / 2.0f, startY + options.size() * stepY + 40.0f, 14.0f, D2D1::ColorF(0.6f, 0.6f, 0.6f), true);
        };

        int choice = MenuRaycasterUtils::renderizarPopupCaixa({}, {}, (int)options.size(), boxBuilder);

        if (choice == -1 || choice == 19) {
            inMenu = false;
        } else if (choice == 0) {
            isGodModeActive = !isGodModeActive.load();
            if (player && isGodModeActive) {
                player->obterAtributosFinais().health += 99999;
                player->strengthrRecalculoCache();
                player->definirVida(player->obterVidaMaxima());
            }
            feedbackMessage = isGodModeActive ? "[!] GODMODE ATIVADO!" : "[!] GODMODE DESATIVADO!";
        } else if (choice == 1) {
            isNoclipActive = !isNoclipActive.load();
            feedbackMessage = isNoclipActive ? "[!] NOCLIP ATIVADO!" : "[!] NOCLIP DESATIVADO!";
        } else if (choice == 2) {
            isOneHitKillActive = !isOneHitKillActive.load();
            feedbackMessage = isOneHitKillActive ? "[!] ONE-HIT KILL ATIVADO!" : "[!] ONE-HIT KILL DESATIVADO!";
        } else if (choice == 3) {
            isSpeedHackActive = !isSpeedHackActive.load();
            feedbackMessage = isSpeedHackActive ? "[!] SUPER VELOCIDADE ATIVADA!" : "[!] SUPER VELOCIDADE DESATIVADA!";
        } else if (choice == 4) { // OURO
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite a quantidade de OURO desejada:", 9);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val < 0) val = 0;
                    if (player && player->obterInventario()) {
                        int currentVal = player->obterInventario()->obterOuro();
                        player->ganharOuro(val - currentVal);
                        feedbackMessage = "[!] Ouro alterado para " + std::to_string(val) + " G!";
                    }
                } catch (...) {}
            }
        } else if (choice == 5) { // NIVEL
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite o NIVEL desejado (1 - 100):", 4);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val >= 1 && val <= 100 && player) {
                        player->definirNivel(val);
                        feedbackMessage = "[!] Nível alterado para " + std::to_string(val) + "!";
                    }
                } catch (...) {}
            }
        } else if (choice == 6) { // VIDA MAXIMA
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite a VIDA MAXIMA desejada:", 7);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val >= 1 && player) {
                        player->obterAtributosFinais().health = val;
                        player->strengthrRecalculoCache();
                        player->definirVida(val);
                        feedbackMessage = "[!] Vida Máxima definida para " + std::to_string(val) + " HP!";
                    }
                } catch (...) {}
            }
        } else if (choice == 7) { // FORCA
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite o valor de FORCA:", 5);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val >= 1 && player) {
                        player->obterAtributosFinais().strength = val;
                        player->strengthrRecalculoCache();
                        feedbackMessage = "[!] Força alterada para " + std::to_string(val) + "!";
                    }
                } catch (...) {}
            }
        } else if (choice == 8) { // DESTREZA
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite o valor de DESTREZA:", 5);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val >= 1 && player) {
                        player->obterAtributosFinais().dexterity = val;
                        player->strengthrRecalculoCache();
                        feedbackMessage = "[!] Destreza alterada para " + std::to_string(val) + "!";
                    }
                } catch (...) {}
            }
        } else if (choice == 9) { // RESISTENCIA
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite o valor de RESISTENCIA:", 5);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val >= 1 && player) {
                        player->obterAtributosFinais().resistance = val;
                        player->strengthrRecalculoCache();
                        feedbackMessage = "[!] Resistência alterada para " + std::to_string(val) + "!";
                    }
                } catch (...) {}
            }
        } else if (choice == 10) { // CONSTITUICAO
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite o valor de CONSTITUICAO:", 5);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val >= 1 && player) {
                        player->obterAtributosFinais().constitution = val;
                        player->strengthrRecalculoCache();
                        feedbackMessage = "[!] Constituição alterada para " + std::to_string(val) + "!";
                    }
                } catch (...) {}
            }
        } else if (choice == 11) { // INTELIGENCIA
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite o valor de INTELIGENCIA:", 5);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val >= 1 && player) {
                        player->obterAtributosFinais().intelligence = val;
                        player->strengthrRecalculoCache();
                        feedbackMessage = "[!] Inteligência alterada para " + std::to_string(val) + "!";
                    }
                } catch (...) {}
            }
        } else if (choice == 12) { // SABEDORIA
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite o valor de SABEDORIA:", 5);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val >= 1 && player) {
                        player->obterAtributosFinais().wisdom = val;
                        player->strengthrRecalculoCache();
                        feedbackMessage = "[!] Sabedoria alterada para " + std::to_string(val) + "!";
                    }
                } catch (...) {}
            }
        } else if (choice == 13) { // XP
            std::string input = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite a quantidade de XP:", 7);
            if (!input.empty()) {
                try {
                    int val = std::stoi(input);
                    if (val >= 0 && player) {
                        player->definirXpAtual(val);
                        feedbackMessage = "[!] XP alterado para " + std::to_string(val) + "!";
                    }
                } catch (...) {}
            }
        } else if (choice == 14) { // RESTAURAR VIDA
            if (player) player->definirVida(player->obterVidaMaxima());
            feedbackMessage = "[!] Vida restaurada para 100% HP!";
        } else if (choice == 15) { // REGISTRAR NPCS
            Diary::instance().registerNPC("Bjorn (Blacksmith)");
            Diary::instance().registerNPC("Franchesco (Merchant)");
            Diary::instance().registerNPC("Morgana (MageNPC)");
            Diary::instance().registerNPC("Anok (Estilista)");
            Diary::instance().registerNPC("Priest Benedito");
            feedbackMessage = "[!] Todos os NPCs foram registrados no Diário!";
        } else if (choice == 16) { // BESTIARIO
            for (const auto& nome : Bestiary::instance().getEnemiesOrderedByDifficulty()) {
                Bestiary::instance().registerFirstSight(nome);
                Bestiary::instance().registerDefeat(nome);
            }
            feedbackMessage = "[!] Todos os inimigos foram desbloqueados no Bestiário!";
        } else if (choice == 17) { // VIAGEM RAPIDA
            Progression::instance().setFlag(Flags::Visited_Forest, true);
            Progression::instance().setFlag(Flags::Visited_KingdomBridge, true);
            Progression::instance().setFlag(Flags::Visited_Kingdom, true);
            Progression::instance().setFlag(Flags::Discovered_Maps, true);
            feedbackMessage = "[!] Viagem rápida desbloqueada para todos os mapas!";
        } else if (choice == 18) { // INICIAR COMBATE PERSONALIZADO
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

                std::vector<std::unique_ptr<Character>> enemies;
                switch (escolhaEspecie) {
                    case 0: enemies = EnemyCreator::createGoblinEnemy(qtd); break;
                    case 1: enemies = EnemyCreator::createSlimeEnemy(qtd); break;
                    case 2: enemies = EnemyCreator::createFairyEnemy(qtd); break;
                    case 3: enemies = EnemyCreator::createExiledOrcEnemy(qtd); break;
                    case 4: enemies = EnemyCreator::createForestAbominationEnemy(qtd); break;
                    case 5: enemies = EnemyCreator::createTrollEnemy(qtd); break;
                    case 6: enemies = EnemyCreator::createMimicEnemy(qtd); break;
                    case 7: enemies = EnemyCreator::createMahoragaEnemy(qtd); break;
                }

                if (!enemies.empty() && player) {
                    std::unique_ptr<ICombateUI> ui = nullptr;
                    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
                        ui = std::make_unique<CombateRaycasterUIImpl>();
                    }
                    Combat combat(player, std::move(enemies), std::move(ui));
                    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
                        combat.setContexto3D(true, MapControllera::obterMatrizDoMapaAtual(),
                                             MapControllera::obterPosCamera3DX(),
                                             MapControllera::obterPosCamera3DY(),
                                             MapControllera::obterAnguloCamera3D(),
                                             MapControllera::obterTituloMapaAtual());
                    }
                    combat.iniciarCombate();
                    feedbackMessage = "[!] Combate finalizado!";
                }
            }
        }
    }
    InputControl::clearBuffer();
}
