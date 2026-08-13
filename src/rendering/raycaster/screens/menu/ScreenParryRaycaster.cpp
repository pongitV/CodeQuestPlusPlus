#include "ScreenParryRaycaster.h"
#include "../utils/MenuRaycasterLayout.h"
#include "../utils/MenuRaycasterUtils.h"
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>

#include "../../../../ui/screens/menu/ScreenMenuBase.h"
#include "../../../../core/utils/DialogFunctions.h"
#include "../../../../core/utils/InputControl.h"
#include "../../../../core/utils/RandomGenerator.h"
#include "../../../../systems/combat/Parry.h"
#include "../../../../rendering/direct-2d/UIRenderer2D.h"
#include "../../../../core/d2d-context/D2DContext.h"
#include "../../../../core/window/GameWindow.h"
#include <windows.h>

namespace {
    inline std::wstring utf8_to_wstring(const std::string& str) {
        return MenuRaycasterUtils::toWStringClean(str);
    }

    int displayTelaComTexto(const std::string& titulo, const std::vector<std::string>& linhas) {
        while (true) {
            if (auto* win = D2DContext::window) win->processarMensagens();
            InputControl::atualizarTeclas();
            char tecla = InputControl::lerTecla();

            auto d2d = D2DContext::renderer;
            if (!d2d) break;

            MenuRaycasterUtils::desenharFundoNativoD2D(true);
            auto rt = d2d->obterRenderTarget();
            if (rt) {
                auto tam = rt->GetSize();
                UIRenderer2D::SetupTransform(d2d, tam.width, tam.height);

                float cx = UIRenderer2D::LOGICAL_WIDTH / 2.0f;
                float cy = UIRenderer2D::LOGICAL_HEIGHT / 2.0f;

                UIDynamicBox box;
                std::wstring wTit = utf8_to_wstring(titulo);

                float textY = cy - (linhas.size() * 15.0f) - 50.0f;
                box.AddText(wTit, cx, textY, 22.0f, D2D1::ColorF(1.0f, 0.84f, 0.0f), true);

                textY += 60.0f;
                for (size_t i = 0; i < linhas.size(); ++i) {
                    std::string plainInfo = linhas[i];
                    if (plainInfo.empty()) { textY += 50.0f; continue; }

                    D2D1_COLOR_F cor = D2D1::ColorF(1.0f, 1.0f, 1.0f);
                    if (linhas[i].find("255;215;0") != std::string::npos) cor = D2D1::ColorF(1.0f, 0.84f, 0.0f);
                    else if (linhas[i].find("180;180;255") != std::string::npos) cor = D2D1::ColorF(0.7f, 0.7f, 1.0f);
                    else if (linhas[i].find("120;120;120") != std::string::npos) cor = D2D1::ColorF(0.5f, 0.5f, 0.5f);
                    else if (linhas[i].find("100;255;100") != std::string::npos) cor = D2D1::ColorF(0.4f, 1.0f, 0.4f);

                    std::wstring wInfo = utf8_to_wstring(plainInfo);
                    box.AddText(wInfo, cx, textY, 16.0f, cor, true);
                    textY += 30.0f;
                }

                box.Render(d2d, D2D1::ColorF(0.0f, 0.0f, 0.0f), 0.85f, 2.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), 20.0f, cx);

                UIRenderer2D::ResetTransform(d2d);
                rt->EndDraw();
            }

            if (tecla == '\r' || tecla == '\n' || tecla == 27) {
                return tecla;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
        return 0;
    }

    void displayContagemRegressiva(int inicio) {
        for (int i = inicio; i > 0; --i) {
            auto start = std::chrono::steady_clock::now();
            while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() < 1000) {
                if (auto* win = D2DContext::window) win->processarMensagens();
                InputControl::atualizarTeclas();
                
                auto d2d = D2DContext::renderer;
                if (!d2d) break;
                MenuRaycasterUtils::desenharFundoNativoD2D(true);
                auto rt = d2d->obterRenderTarget();
                if (rt) {
                    auto tam = rt->GetSize();
                    UIRenderer2D::SetupTransform(d2d, tam.width, tam.height);

                    float cx = UIRenderer2D::LOGICAL_WIDTH / 2.0f;
                    float cy = UIRenderer2D::LOGICAL_HEIGHT / 2.0f;
                    
                    UIDynamicBox box;
                    std::wstring num = std::to_wstring(i);
                    box.AddText(num, cx, cy - 60.0f, 120.0f, D2D1::ColorF(1.0f, 20.0f, 0.4f), true);
                    box.Render(d2d, D2D1::ColorF(0.0f, 0.0f, 0.0f), 0.85f, 2.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), 20.0f, cx);

                    UIRenderer2D::ResetTransform(d2d);
                    rt->EndDraw();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }
        }
        auto start = std::chrono::steady_clock::now();
        while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() < 800) {
            if (auto* win = D2DContext::window) win->processarMensagens();
            InputControl::atualizarTeclas();
            
            auto d2d = D2DContext::renderer;
            if (!d2d) break;
            MenuRaycasterUtils::desenharFundoNativoD2D(true);
            auto rt = d2d->obterRenderTarget();
            if (rt) {
                auto tam = rt->GetSize();
                UIRenderer2D::SetupTransform(d2d, tam.width, tam.height);

                float cx = UIRenderer2D::LOGICAL_WIDTH / 2.0f;
                float cy = UIRenderer2D::LOGICAL_HEIGHT / 2.0f;
                
                UIDynamicBox box;
                box.AddText(L"VAI!", cx, cy - 60.0f, 120.0f, D2D1::ColorF(0.4f, 20.0f, 0.4f), true);
                box.Render(d2d, D2D1::ColorF(0.0f, 0.0f, 0.0f), 0.85f, 2.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), 20.0f, cx);

                UIRenderer2D::ResetTransform(d2d);
                rt->EndDraw();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

    void rodarTutorialMovimento() {
        struct Nivel { std::string nome; int difficulty; };
        std::vector<Nivel> niveis = {
            {"NIVEL 1 - Facil", 3},
            {"NIVEL 2 - Medio", 6},
            {"NIVEL 3 - Dificil", 10},
            {"NIVEL EXTRA - Desafio", 14}
        };

        for (size_t i = 0; i < niveis.size(); ++i) {
            std::vector<std::string> intro = {
                niveis[i].nome,
                "",
                "Pressione ESPACO quando o marcador estiver na zona verde!",
                "",
                "Pressione ENTER para comecar..."
            };
            displayTelaComTexto("TUTORIAL DE PARRY MOVIMENTO", intro);
            displayContagemRegressiva(3);

            int acertos = 0;
            for (int teste = 1; teste <= 5; ++teste) {
                bool acertouParry = false;
                std::string msgResultado = "";
                D2D1_COLOR_F corResultado = D2D1::ColorF(1,1,1);

                Parry::onUpdateScreen = [&]() {
                    auto d2d = D2DContext::renderer;
                    if (!d2d) return;
                    MenuRaycasterUtils::desenharFundoNativoD2D(true);
                    auto rt = d2d->obterRenderTarget();
                    if (rt) {
                        auto tam = rt->GetSize();
                        UIRenderer2D::SetupTransform(d2d, tam.width, tam.height);

                        float cx = UIRenderer2D::LOGICAL_WIDTH / 2.0f;
                        float cy = UIRenderer2D::LOGICAL_HEIGHT / 2.0f;

                        float barW = 300.0f;
                        float barH = 30.0f;
                        float barLeft = cx - barW / 2.0f;
                        float barTop = cy + 50.0f;

                        UIDynamicBox box;
                        box.AddText(L"PARRY DIRECIONAL - DEMO", cx, cy - 120.0f, 22.0f, D2D1::ColorF(1.0f, 0.84f, 0.0f), true);
                        std::wstring sub = utf8_to_wstring("Teste " + std::to_string(teste) + "/5 - " + niveis[i].nome);
                        box.AddText(sub, cx, cy - 70.0f, 16.0f, D2D1::ColorF(0.7f, 0.7f, 1.0f), true);

                        std::wstring mainText;
                        D2D1_COLOR_F mainColor = D2D1::ColorF(1,1,1);
                        if (!msgResultado.empty()) {
                            mainText = utf8_to_wstring(msgResultado);
                            mainColor = corResultado;
                        } else if (!Parry::minigameMessage.empty()) {
                            mainText = utf8_to_wstring(Parry::minigameMessage);
                        }
                        if (!mainText.empty()) {
                            box.AddText(mainText, cx, cy + 5.0f, 20.0f, mainColor, true);
                        }

                        float unitW = Parry::barSize > 0 ? barW / Parry::barSize : 1.0f;
                        if (!msgResultado.empty()) {
                            D2D1_COLOR_F barColor = acertouParry
                                ? D2D1::ColorF(0.0f, 0.7f, 0.0f)
                                : D2D1::ColorF(0.7f, 0.0f, 0.0f);
                            box.AddRect(barLeft, barTop, barW, barH, barColor);
                        } else if (Parry::barSize > 0 && Parry::cursorPos >= 0) {
                            box.AddRect(barLeft, barTop, barW, barH, D2D1::ColorF(0.2f, 0.2f, 0.2f));
                            float ssLeft = barLeft + (Parry::sweetSpotCenter - Parry::sweetSpotSize / 2) * unitW;
                            float ssW = Parry::sweetSpotSize * unitW;
                            box.AddRect(ssLeft, barTop, ssW, barH, D2D1::ColorF(1.0f, 0.85f, 0.0f)); // Bordas amarelas

                            float centerLeft = barLeft + (Parry::sweetSpotCenter - Parry::sweetSpotSize / 4) * unitW;
                            float centerW = (Parry::sweetSpotSize / 2) * unitW;
                            box.AddRect(centerLeft, barTop, centerW, barH, D2D1::ColorF(0.0f, 0.6f, 0.0f)); // Centro verde
                            float curLeft = barLeft + Parry::cursorPos * unitW;
                            box.AddRect(curLeft, barTop, unitW, barH, D2D1::ColorF(0.0f, 0.8f, 1.0f));
                        }

                        box.Render(d2d, D2D1::ColorF(0.0f, 0.0f, 0.0f), 0.85f, 2.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), 20.0f, cx);

                        UIRenderer2D::ResetTransform(d2d);
                        rt->EndDraw();
                    }
                };

                int dmgRed = 0;
                float speedMul = (i == 0) ? 1.0f : (i == 1) ? 1.5f : 2.0f;
                int ssSize = (i == 0) ? 6 : (i == 1) ? 4 : 2;
                bool sucesso = Parry::executarMinigameMovimento(niveis[i].difficulty, 100, dmgRed, speedMul, ssSize);
                acertouParry = sucesso;

                if (sucesso) {
                    if (dmgRed == 100) {
                        msgResultado = "Parry Perfeito! (dano anulado)";
                        corResultado = D2D1::ColorF(0.4f, 1.0f, 0.4f);
                    } else {
                        msgResultado = "Parry Efetivo! (dano reduzido)";
                        corResultado = D2D1::ColorF(1.0f, 1.0f, 0.4f);
                    }
                    acertos++;
                } else {
                    msgResultado = "Parry Falhou!";
                    corResultado = D2D1::ColorF(1.0f, 0.4f, 0.4f);
                }
                
                Parry::minigameMessage = "";
                Parry::minigameBar = "";
                
                auto fimTeste = std::chrono::steady_clock::now();
                while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - fimTeste).count() < 1200) {
                    if (auto* win = D2DContext::window) win->processarMensagens();
                    Parry::onUpdateScreen();
                    std::this_thread::sleep_for(std::chrono::milliseconds(16));
                }
            }
            Parry::onUpdateScreen = nullptr;

            std::vector<std::string> resultado = {
                "Resultado: " + std::to_string(acertos) + "/5 acertos.",
                "",
                "Pressione ENTER para continuar..."
            };
            displayTelaComTexto("TUTORIAL DE PARRY MOVIMENTO", resultado);
        }

        std::vector<std::string> concluido = {
            "Tutorial de Parry Movimento concluido!",
            "",
            "Pressione ENTER para continuar..."
        };
        displayTelaComTexto("PARABENS!", concluido);
    }

    void rodarTutorialDigitacao() {
        struct Nivel { std::string nome; int difficulty; int digitos; float speedMul; int danoBase; };
        std::vector<Nivel> niveis = {
            {"NIVEL 1 - Facil",    3, 4, 0.7f, 30},
            {"NIVEL 2 - Medio",    6, 5, 1.0f, 50},
            {"NIVEL 3 - Dificil", 10, 6, 1.5f, 70},
            {"NIVEL EXTRA - Desafio", 14, 7, 2.0f, 100}
        };

        for (size_t i = 0; i < niveis.size(); ++i) {
            std::vector<std::string> intro = {
                niveis[i].nome,
                "",
                "Digite a sequencia de numeros que aparecer!",
                std::to_string(niveis[i].digitos) + " digitos | dano base " + std::to_string(niveis[i].danoBase),
                "",
                "Pressione ENTER para comecar..."
            };
            displayTelaComTexto("TUTORIAL DE PARRY DIGITACAO", intro);
            displayContagemRegressiva(3);

            int acertos = 0;
            for (int teste = 1; teste <= 5; ++teste) {
                std::string msgResultado = "";
                D2D1_COLOR_F corResultado = D2D1::ColorF(1,1,1);

                Parry::onUpdateScreen = [&]() {
                    auto d2d = D2DContext::renderer;
                    if (!d2d) return;
                    MenuRaycasterUtils::desenharFundoNativoD2D(true);
                    auto rt = d2d->obterRenderTarget();
                    if (rt) {
                        auto tam = rt->GetSize();
                        UIRenderer2D::SetupTransform(d2d, tam.width, tam.height);

                        float cx = UIRenderer2D::LOGICAL_WIDTH / 2.0f;
                        float cy = UIRenderer2D::LOGICAL_HEIGHT / 2.0f;

                        UIDynamicBox box;
                        box.AddText(L"PARRY DE DIGITACAO - DEMO", cx, cy - 130.0f, 22.0f, D2D1::ColorF(1.0f, 0.84f, 0.0f), true);
                        std::wstring sub = utf8_to_wstring("Teste " + std::to_string(teste) + "/5 - " + niveis[i].nome);
                        box.AddText(sub, cx, cy - 70.0f, 16.0f, D2D1::ColorF(0.7f, 0.7f, 1.0f), true);

                        bool gameActive = msgResultado.empty();
                        if (gameActive && !Parry::minigameMessage.empty()) {
                            std::wstring msg = utf8_to_wstring(Parry::minigameMessage);
                            box.AddText(msg, cx, cy + 10.0f, 18.0f, D2D1::ColorF(1,1,1), true);
                        } else {
                            box.AddText(L"", cx, cy + 10.0f, 18.0f, D2D1::ColorF(1,1,1), true, false);
                        }
                        if (gameActive && !Parry::minigameBar.empty()) {
                            std::wstring bar = utf8_to_wstring(Parry::minigameBar);
                            box.AddText(bar, cx, cy + 60.0f, 16.0f, D2D1::ColorF(0.4f, 20.0f, 0.4f), true);
                        } else {
                            box.AddText(L"", cx, cy + 60.0f, 16.0f, D2D1::ColorF(0.4f, 20.0f, 0.4f), true, false);
                        }
                        if (!msgResultado.empty()) {
                            std::wstring res = utf8_to_wstring(msgResultado);
                            box.AddText(res, cx, cy + 110.0f, 18.0f, corResultado, true);
                        } else {
                            box.AddText(L"", cx, cy + 110.0f, 18.0f, D2D1::ColorF(1,1,1), true, false);
                        }

                        box.Render(d2d, D2D1::ColorF(0.0f, 0.0f, 0.0f), 0.85f, 2.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), 20.0f, cx);

                        UIRenderer2D::ResetTransform(d2d);
                        rt->EndDraw();
                    }
                };

                int dmgRed = 0;
                bool sucesso = Parry::executarMinigameDigitacao(niveis[i].difficulty, niveis[i].danoBase, dmgRed, niveis[i].speedMul, niveis[i].digitos);

                if (sucesso) {
                    if (dmgRed == niveis[i].danoBase) {
                        msgResultado = "Parry Perfeito! (dano anulado)";
                        corResultado = D2D1::ColorF(0.4f, 1.0f, 0.4f);
                    } else {
                        msgResultado = "Parry Efetivo! (dano reduzido)";
                        corResultado = D2D1::ColorF(1.0f, 1.0f, 0.4f);
                    }
                    acertos++;
                } else {
                    msgResultado = "Parry Falhou!";
                    corResultado = D2D1::ColorF(1.0f, 0.4f, 0.4f);
                }

                Parry::minigameMessage = "";
                Parry::minigameBar = "";
                
                auto fimTeste = std::chrono::steady_clock::now();
                while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - fimTeste).count() < 1200) {
                    if (auto* win = D2DContext::window) win->processarMensagens();
                    Parry::onUpdateScreen();
                    std::this_thread::sleep_for(std::chrono::milliseconds(16));
                }
            }
            Parry::onUpdateScreen = nullptr;

            std::vector<std::string> resultado = {
                "Resultado: " + std::to_string(acertos) + "/5 acertos.",
                "",
                "Pressione ENTER para continuar..."
            };
            displayTelaComTexto("TUTORIAL DE PARRY DIGITACAO", resultado);
        }

        std::vector<std::string> concluido = {
            "Tutorial de Parry Digitacao concluido!",
            "",
            "Pressione ENTER para continuar..."
        };
        displayTelaComTexto("PARABENS!", concluido);
        InputControl::lerTecla();
        InputControl::limparBuffer();
    }

}

TelaParry::Resultado TelaParryRaycaster::display(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse) {
    std::vector<std::string> opcoes = {
        "PARRY DESLIGADO",
        "PARRY MOVIMENTO (Barra deslizante)",
        "PARRY DIGITACAO (Digitar por tempo)",
        "VOLTAR"
    };

    int selecaoAtual = 0;
    MenuRaycasterUtils::cachearBackground3D("Village", nullptr);

    while (true) {
        if (auto* win = D2DContext::window) {
            win->processarMensagens();
        }
        InputControl::atualizarTeclas();
        char tecla = InputControl::lerTecla();

        auto d2d = D2DContext::renderer;
        if (!d2d) break;

        MenuRaycasterUtils::desenharFundoNativoD2D(/*abrirFrame=*/true);

        auto rt = d2d->obterRenderTarget();
        if (rt) {
            auto tam = rt->GetSize();
            UIRenderer2D::SetupTransform(d2d, tam.width, tam.height);

            float logicalW = UIRenderer2D::LOGICAL_WIDTH;
            float logicalH = UIRenderer2D::LOGICAL_HEIGHT;

            // Info Box
            std::string infoStr = nomeJogador + " | " + nomeRaca + " | " + nomeClasse;
            std::wstring wInfo(infoStr.begin(), infoStr.end());
            
            UIDynamicBox infoBox;
            infoBox.AddText(wInfo, logicalW / 2.0f, 100.0f, 16.0f, D2D1::ColorF(1.0f, 20.0f, 1.0f), true);
            infoBox.Render(d2d, D2D1::ColorF(0.0f, 0.0f, 0.0f), 0.85f, 2.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), 20.0f, logicalW / 2.0f);

            // Main Box
            UIDynamicBox mainBox;
            float opY = 250.0f;
            for (int i = 0; i < (int)opcoes.size(); ++i) {
                std::string opStr = (i == selecaoAtual ? "> " : "  ") + opcoes[i];
                std::wstring wOpStr = utf8_to_wstring(opStr);
                
                D2D1_COLOR_F corText = (i == selecaoAtual) ? D2D1::ColorF(0.4f, 1.0f, 0.4f) : D2D1::ColorF(0.5f, 0.5f, 0.5f);
                mainBox.AddText(wOpStr, logicalW / 2.0f, opY + i * 60.0f, 18.0f, corText, true);
            }

            mainBox.Render(d2d, D2D1::ColorF(0.0f, 0.0f, 0.0f), 0.85f, 2.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), 20.0f, logicalW / 2.0f);

            UIRenderer2D::ResetTransform(d2d);
            rt->EndDraw();
        }

        if (tecla == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            continue;
        }

        if (tecla == 'w' || tecla == 'W') {
            selecaoAtual = (selecaoAtual - 1 + (int)opcoes.size()) % (int)opcoes.size();
        } else if (tecla == 's' || tecla == 'S') {
            selecaoAtual = (selecaoAtual + 1) % (int)opcoes.size();
        } else if (tecla == '\r' || tecla == '\n') {
            if (selecaoAtual == 3) {
                TelaParry::Resultado r;
                r.voltou = true;
                return r;
            }

            TelaParry::Resultado r;
            if (selecaoAtual == 1) {
                r.modo = TelaParry::Resultado::Modo::Movimento;
                std::vector<std::string> explicacao = {
                    "Uma barra horizontal com uma zona verde central surgira na tela.",
                    "Um cursor percorrera a barra da esquerda para a direita.",
                    "Pressione ESPACO no momento exato em que o cursor estiver na zona verde!",
                    "",
                    "Pressione ENTER para iniciar o tutorial...",
                    "Pressione ESC para pular o tutorial..."
                };
                int t = displayTelaComTexto("PARRY MOVIMENTO - TUTORIAL", explicacao);
                if (t != 27) {
                    rodarTutorialMovimento();
                }
            } else if (selecaoAtual == 2) {
                r.modo = TelaParry::Resultado::Modo::Digitacao;
                std::vector<std::string> explicacao = {
                    "Uma sequencia de numeros aparecera na tela com um limite de tempo.",
                    "Digite os numeros rapidamente na sequencia correta e pressione ENTER.",
                    "Se for rapido o suficiente, o dano sera reduzido ou anulado!",
                    "",
                    "Pressione ENTER para iniciar o tutorial...",
                    "Pressione ESC para pular o tutorial..."
                };
                int t = displayTelaComTexto("PARRY DIGITACAO - TUTORIAL", explicacao);
                if (t != 27) {
                    rodarTutorialDigitacao();
                }
            } else {
                r.modo = TelaParry::Resultado::Modo::Desligado;
            }
            return r;
        } else if (tecla == 27) {
            TelaParry::Resultado r;
            r.voltou = true;
            return r;
        }
    }
    
    TelaParry::Resultado r_fallback;
    r_fallback.voltou = true;
    return r_fallback;
}

