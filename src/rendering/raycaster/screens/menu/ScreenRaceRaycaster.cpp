#include "ScreenRaceRaycaster.h"
#include "../utils/MenuRaycasterLayout.h"
#include "../utils/MenuRaycasterUtils.h"
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <memory>
#include <algorithm>

#include "../../../../ui/screens/menu/ScreenMenuBase.h"
#include "../../../../core/utils/DialogFunctions.h"
#include "../../../../core/utils/InputControl.h"
#include "../../../../entities/races/RaceFactory.h"
#include "../../../../entities/character/Character.h"

#include "../../../../core/d2d-context/D2DContext.h"
#include "../../../../rendering/direct-2d/D2DRenderer.h"
#include "../../../../core/window/GameWindow.h"
#include "../../../../rendering/direct-2d/UIRenderer2D.h"
#include "../../../../rendering/direct-2d/GerenciadorTexturas2D.h"

struct OpcaoRaca { RaceType tipo; std::string nome; };

TelaRaca::Resultado TelaRacaRaycaster::display(const std::string& nomeJogador) {
    std::vector<OpcaoRaca> opcoesGerais;
    for (auto t : FabricaRacas::obterRacasJogaveis()) {
        auto temp = FabricaRacas::createRace(t);
        opcoesGerais.push_back({t, temp->getRaceName()});
    }
    std::sort(opcoesGerais.begin(), opcoesGerais.end(), [](const OpcaoRaca& a, const OpcaoRaca& b) { return a.nome < b.nome; });

    int selecaoAtual = 0;

    InputControl::limparBuffer();
    MenuRaycasterUtils::cachearBackground3D("Village", nullptr);

    while (true) {
        if (auto* win = D2DContext::window) {
            win->processarMensagens();
        }
        InputControl::atualizarTeclas();

        char tecla = InputControl::lerTecla();

        auto d2d = D2DContext::renderer;
        if (!d2d) break;

        MenuRaycasterUtils::desenharFundoNativoD2D(true);

        bool isVoltar = (selecaoAtual >= (int)opcoesGerais.size());
        int totalOpcoes = (int)opcoesGerais.size() + 1;

        auto rt = d2d->obterRenderTarget();
        if (rt) {
            auto tam = rt->GetSize();
            UIRenderer2D::SetupTransform(d2d, tam.width, tam.height);

            float logicalW = UIRenderer2D::LOGICAL_WIDTH;
            float logicalH = UIRenderer2D::LOGICAL_HEIGHT;

            std::string titulo = "SELECIONE SUA RACA - " + nomeJogador;
            std::wstring wTitulo(titulo.begin(), titulo.end());
            
            UIDynamicBox topBox;
            topBox.AddText(wTitulo, logicalW / 2.0f, 40.0f, 16.0f, D2D1::ColorF(1.0f, 20.0f, 1.0f), true);
            topBox.Render(d2d, D2D1::ColorF(0.0f, 0.0f, 0.0f), 0.85f, 2.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), 20.0f, logicalW / 2.0f);

            float listX = 150.0f;
            float listY = 200.0f;
            for (int i = 0; i < totalOpcoes; ++i) {
                std::string nomeOpcao = (i == (int)opcoesGerais.size()) ? "VOLTAR" : opcoesGerais[i].nome;
                std::wstring wNomeOpcao(nomeOpcao.begin(), nomeOpcao.end());
                std::wstring prefix = (i == selecaoAtual) ? L"> " : L"  ";
                D2D1_COLOR_F cor = (i == selecaoAtual) ? D2D1::ColorF(0.0f, 1.0f, 0.0f) : D2D1::ColorF(0.7f, 0.7f, 0.7f);
                UIRenderer2D::DrawTextNative(d2d, prefix + wNomeOpcao, listX, listY + i * 40.0f, cor, 18.0f, false);
            }

            auto race = isVoltar ? nullptr : FabricaRacas::createRace(opcoesGerais[selecaoAtual].tipo);
            if (!isVoltar && race) {
                float spriteWidth = 280.0f;
                float spriteHeight = 280.0f;
                float arteX = logicalW / 2.0f - (spriteWidth / 2.0f);
                float arteY = 220.0f;

                std::string spritePath = race->getRaceSpritePath();
                GerenciadorTexturas2D::desenharImagem(d2d->obterRenderTarget(), spritePath, arteX, arteY, spriteWidth, spriteHeight);

                Attributes atr = race->getRaceAttributes();
                float rightX = logicalW - 550.0f;
                float rightY = 200.0f;

                UIRenderer2D::DrawTextNative(d2d, L"[ATRIBUTOS]", rightX, rightY, D2D1::ColorF(0.4f, 0.8f, 1.0f), 22.0f, false);
                rightY += 35.0f;
                UIRenderer2D::DrawTextNative(d2d, L"  HP " + std::to_wstring(atr.health), rightX, rightY, D2D1::ColorF(0.7f, 0.7f, 0.7f), 16.0f, false);
                rightY += 30.0f;

                auto desenharAtributo = [&](const std::wstring& label, int val, D2D1_COLOR_F cor) {
                    std::wstring sinal = (val >= 0 ? L"+" : L"");
                    UIRenderer2D::DrawTextNative(d2d, L"  " + label + L" " + sinal + std::to_wstring(val), rightX, rightY, cor, 16.0f, false);
                    rightY += 30.0f;
                };

                desenharAtributo(L"For", atr.strength, D2D1::ColorF(1.0f, 0.6f, 0.6f));
                desenharAtributo(L"Des", atr.dexterity, D2D1::ColorF(0.6f, 1.0f, 0.6f));
                desenharAtributo(L"Res", atr.resistance, D2D1::ColorF(0.6f, 0.6f, 1.0f));
                desenharAtributo(L"Con", atr.constitution, D2D1::ColorF(0.0f, 1.0f, 1.0f));
                desenharAtributo(L"Int", atr.intelligence, D2D1::ColorF(0.4f, 0.8f, 1.0f));
                desenharAtributo(L"Sab", atr.wisdom, D2D1::ColorF(1.0f, 0.84f, 0.0f));
                
                rightY += 20.0f;
                UIRenderer2D::DrawTextNative(d2d, L"[PASSIVA]", rightX, rightY, D2D1::ColorF(0.4f, 0.8f, 1.0f), 22.0f, false);
                rightY += 35.0f;
                std::string passNome = race->getRaceAbilityName();
                UIRenderer2D::DrawTextNative(d2d, L"  " + std::wstring(passNome.begin(), passNome.end()), rightX, rightY, D2D1::ColorF(1.0f, 1.0f, 1.0f), 16.0f, false);
                rightY += 30.0f;
                
                std::string desc = race->getRaceAbilityDescription();
                std::string linhaP;
                std::istringstream passStream(desc);
                while (std::getline(passStream, linhaP)) {
                    if (linhaP.empty()) continue;
                    while (linhaP.length() > 55) {
                        size_t brk = linhaP.rfind(' ', 55);
                        if (brk == std::string::npos) brk = 55;
                        { std::string part = linhaP.substr(0, brk);
                        UIRenderer2D::DrawTextNative(d2d, L"  " + std::wstring(part.begin(), part.end()), rightX, rightY, D2D1::ColorF(0.7f, 0.7f, 0.7f), 16.0f, false); }
                        rightY += 25.0f;
                        linhaP = linhaP.substr(brk + (linhaP[brk] == ' ' ? 1 : 0));
                    }
                    UIRenderer2D::DrawTextNative(d2d, L"  " + std::wstring(linhaP.begin(), linhaP.end()), rightX, rightY, D2D1::ColorF(0.7f, 0.7f, 0.7f), 16.0f, false);
                    rightY += 25.0f;
                }
            }

            UIRenderer2D::ResetTransform(d2d);
            rt->EndDraw();
        }

        // Process input captured at the top of the loop
        if (tecla == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            continue;
        }

        if (tecla == 'w' || tecla == 'W') {
            selecaoAtual = (selecaoAtual - 1 + totalOpcoes) % totalOpcoes;
        } else if (tecla == 's' || tecla == 'S') {
            selecaoAtual = (selecaoAtual + 1) % totalOpcoes;
        } else if (tecla == '\r' || tecla == '\n') {
            if (isVoltar) {
                TelaRaca::Resultado r;
                r.voltou = true;
                return r;
            }
            TelaRaca::Resultado r;
            r.indice = selecaoAtual;
            r.nome = opcoesGerais[selecaoAtual].nome;
            r.racaSelecionada = opcoesGerais[selecaoAtual].tipo;
            return r;
        } else if (tecla == 27) { // ESC = voltar
            TelaRaca::Resultado r;
            r.voltou = true;
            return r;
        }
    }
    
    TelaRaca::Resultado r_fallback;
    r_fallback.voltou = true;
    return r_fallback;
}
