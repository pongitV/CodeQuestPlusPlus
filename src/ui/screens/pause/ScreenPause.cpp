#include "ScreenPause.h"
#include "../utils/ScreenRegistry.h"
#include <iostream>
#include "../../../entities/character/Character.h"
#include "../../UIManager.h"
#include "../../../core/utils/InputControl.h"
#include "../../../core/utils/Color.h"
#include "../../../rendering/raycaster/screens/utils/MenuRaycasterUtils.h"

static int obterEscolhaMenuPause() {
    return GerenciadorPerspectiva::obterPauseUI().renderizarMenuPause();
}

static int obterEscolhaConfiguracoes(Character* jogador) {
    return GerenciadorPerspectiva::obterPauseUI().renderizarMenuConfiguracoes(jogador);
}

static int obterEscolhaAparencia(Character* jogador) {
    return GerenciadorPerspectiva::obterPauseUI().renderizarMenuAparencia(jogador);
}

static int obterEscolhaFundo(int corFundoAtualIndex) {
    return GerenciadorPerspectiva::obterPauseUI().renderizarMenuFundo(corFundoAtualIndex);
}

static int obterEscolhaSensibilidade(int percX, int percY) {
    return GerenciadorPerspectiva::obterPauseUI().renderizarMenuSensibilidade(percX, percY);
}

void TelaPause::display(Character* jogador) {
    int corFundoAtualIndex = 0;
    bool continuar = true;

    while (continuar && !jogador->obterVoltarProMenu()) {
        int escolha = obterEscolhaMenuPause();

        if (escolha == 0) {
            continuar = false;
        } else if (escolha == 1) {
            bool configAberta = true;
            while (configAberta) {
                int confEscolha = obterEscolhaConfiguracoes(jogador);

                if (confEscolha == 0) {
                    int difficultyAtual = static_cast<int>(jogador->obterDificuldade());
                    difficultyAtual++;
                    if (difficultyAtual > 3) difficultyAtual = 1;
                    jogador->definirDificuldade(static_cast<DificuldadeJogo>(difficultyAtual));
                } else if (confEscolha == 1) {
                    jogador->definirParryAtivado(!jogador->obterParryAtivado());
                } else if (confEscolha == 2) {
                    jogador->definirParryModerno(!jogador->obterParryModerno());
                } else if (confEscolha == 3) {
                    bool aparenciaAberta = true;
                    while (aparenciaAberta) {
                        int apEscolha = obterEscolhaAparencia(jogador);

                        if (apEscolha == 0) {
                            // Ignorado na versao Direct2D por enquanto
                        } else if (apEscolha == 1) {
                            std::string novoIcone = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite o novo icone (1 caractere): ", 1);
                            if (!novoIcone.empty() && novoIcone[0] != ' ') {
                                // Ignorado na versao Direct2D por enquanto
                            }
                        } else {
                            aparenciaAberta = false;
                        }
                    }
                } else if (confEscolha == 4) {
                    bool fundoAberto = true;
                    while (fundoAberto) {
                        int fundoEscolha = obterEscolhaFundo(corFundoAtualIndex);

                        if (fundoEscolha >= 0 && fundoEscolha <= 5) {
                            corFundoAtualIndex = fundoEscolha;
                            std::string hexColor;
                            switch (fundoEscolha) {
                                case 0: hexColor = "#0C0C0C"; break; case 1: hexColor = "#1A1A1A"; break;
                                case 2: hexColor = "#000022"; break; case 3: hexColor = "#220000"; break;
                                case 4: hexColor = "#002200"; break; case 5: hexColor = "#220022"; break;
                            }
                            // Em D2D a cor do fundo da ui talvez nao use Aparencia, mantendo a logica por compatibilidade visual
                        } else {
                            fundoAberto = false;
                        }
                    }
                } else if (confEscolha == 5) {
                    bool sensibilidadeAberta = true;
                    while (sensibilidadeAberta) {
                        int percX = (int)((GerenciadorPerspectiva::obterSensibilidadeMouseX() / 0.002f) * 100);
                        int percY = (int)((GerenciadorPerspectiva::obterSensibilidadeMouseY() / 0.008f) * 100);

                        int sensEscolha = obterEscolhaSensibilidade(percX, percY);

                        if (sensEscolha == 0) {
                            std::string entrada = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite o novo valor em porcentagem (ex: 50, 100, 150): ", 4);
                            try {
                                int novoValor = std::stoi(entrada);
                                if (novoValor > 0) GerenciadorPerspectiva::definirSensibilidadeMouse((novoValor / 100.0f) * 0.002f, GerenciadorPerspectiva::obterSensibilidadeMouseY());
                            } catch (...) {}
                        } else if (sensEscolha == 1) {
                            std::string entrada = MenuRaycasterUtils::lerEntradaTextoD2D(L"Digite o novo valor em porcentagem (ex: 50, 100, 150): ", 4);
                            try {
                                int novoValor = std::stoi(entrada);
                                if (novoValor > 0) GerenciadorPerspectiva::definirSensibilidadeMouse(GerenciadorPerspectiva::obterSensibilidadeMouseX(), (novoValor / 100.0f) * 0.008f);
                            } catch (...) {}
                        } else {
                            sensibilidadeAberta = false;
                        }
                    }
                } else {
                    configAberta = false;
                }
            }
        } else if (escolha == 2) {
            if (RegistroTelas::confirmarSaida()) {
                std::exit(0);
            }
        }
    }
}
