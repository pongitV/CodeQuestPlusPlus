#include "ScreenClassRaycaster.h"
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
#include "../../../../entities/classes/ClassFactory.h"
#include "../../../../entities/character/Character.h"

#include "../../../../core/d2d-context/D2DContext.h"
#include "../../../../rendering/direct-2d/D2DRenderer.h"
#include "../../../../core/window/GameWindow.h"
#include "../../../../rendering/direct-2d/UIRenderer2D.h"
#include "../../../../rendering/direct-2d/GerenciadorTexturas2D.h"

struct OpcaoClasse { ClassType tipo; std::string nome; };

TelaClasse::Resultado TelaClasseRaycaster::display(const std::string& nomeJogador, const std::string& nomeRaca) {
    std::vector<OpcaoClasse> opcoesGerais;
    for (auto t : ClassFactory::obterClassesJogaveis()) {
        auto temp = ClassFactory::createClass(t);
        opcoesGerais.push_back({t, temp->getNameClasse()});
    }
    std::sort(opcoesGerais.begin(), opcoesGerais.end(), [](const OpcaoClasse& a, const OpcaoClasse& b) { return a.nome < b.nome; });

    int selecaoAtual = 0;

    InputControl::limparBuffer();
    MenuRaycasterUtils::cachearBackground3D("Village", nullptr);

    while (true) {
        if (auto* win = D2DContext::window) {
            win->processarMensagens();
        }
        InputControl::atualizarTeclas();

        // Read input immediately after atualizarTeclas (rising-edge capture)
        char tecla = InputControl::lerTecla();

        auto d2d = D2DContext::renderer;
        if (!d2d) break;

        MenuRaycasterUtils::desenharFundoNativoD2D(/*abrirFrame=*/true);

        bool isVoltar = (selecaoAtual >= (int)opcoesGerais.size());
        auto classe = isVoltar ? nullptr : ClassFactory::createClass(opcoesGerais[selecaoAtual].tipo);
        int totalOpcoes = (int)opcoesGerais.size() + 1;

        auto rt = d2d->obterRenderTarget();
        if (rt) {
            auto tam = rt->GetSize();
            UIRenderer2D::SetupTransform(d2d, tam.width, tam.height);

            float logicalW = UIRenderer2D::LOGICAL_WIDTH;
            float logicalH = UIRenderer2D::LOGICAL_HEIGHT;

            // Desenha caixa superior com raca e nome
            std::string infoBoxStr = nomeJogador + " | " + nomeRaca;
            std::wstring wInfoBox(infoBoxStr.begin(), infoBoxStr.end());
            
            UIDynamicBox topBox;
            topBox.AddText(wInfoBox, logicalW / 2.0f, 40.0f, 16.0f, D2D1::ColorF(1.0f, 20.0f, 1.0f), true);
            topBox.Render(d2d, D2D1::ColorF(0.0f, 0.0f, 0.0f), 0.85f, 2.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), 20.0f, logicalW / 2.0f);

            // Left panel: Classes List
            float listX = 150.0f;
            float listY = 200.0f;
            for (int i = 0; i < totalOpcoes; ++i) {
                std::string nomeOpcao = (i == (int)opcoesGerais.size()) ? "VOLTAR" : opcoesGerais[i].nome;
                std::wstring wNomeOpcao(nomeOpcao.begin(), nomeOpcao.end());
                std::wstring prefix = (i == selecaoAtual) ? L"> " : L"  ";
                D2D1_COLOR_F cor = (i == selecaoAtual) ? D2D1::ColorF(0.0f, 1.0f, 0.0f) : D2D1::ColorF(0.7f, 0.7f, 0.7f);
                UIRenderer2D::DrawTextNative(d2d, prefix + wNomeOpcao, listX, listY + i * 40.0f, cor, 18.0f, false);
            }

            if (!isVoltar && classe != nullptr) {
                float spriteWidth = 280.0f;
                float spriteHeight = 280.0f;
                float arteX = logicalW / 2.0f - (spriteWidth / 2.0f);
                float arteY = 220.0f;

                std::string spritePath = classe->obterCaminhoSprite();
                GerenciadorTexturas2D::desenharImagem(d2d->obterRenderTarget(), spritePath, arteX, arteY, spriteWidth, spriteHeight);

                // Painel direito: atributos e habilidades
                Attributes atr = classe->obterAtributosClasse();
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
                UIRenderer2D::DrawTextNative(d2d, L"[ATIVA]", rightX, rightY, D2D1::ColorF(0.4f, 0.8f, 1.0f), 22.0f, false);
                rightY += 35.0f;
                std::string habNome = classe->getNameHabilidadeClasse();
                UIRenderer2D::DrawTextNative(d2d, L"  " + std::wstring(habNome.begin(), habNome.end()), rightX, rightY, D2D1::ColorF(1.0f, 1.0f, 1.0f), 16.0f, false);
                rightY += 30.0f;
                
                std::string linhaDesc;
                {
                    std::istringstream descStream(classe->getClassAbilityDescription());
                    while (std::getline(descStream, linhaDesc)) {
                        if (linhaDesc.empty()) continue;
                        while (linhaDesc.length() > 55) {
                            size_t brk = linhaDesc.rfind(' ', 55);
                            if (brk == std::string::npos) brk = 55;
                            { std::string part = linhaDesc.substr(0, brk);
                            UIRenderer2D::DrawTextNative(d2d, L"  " + std::wstring(part.begin(), part.end()), rightX, rightY, D2D1::ColorF(0.7f, 0.7f, 0.7f), 16.0f, false); }
                            rightY += 25.0f;
                            linhaDesc = linhaDesc.substr(brk + (linhaDesc[brk] == ' ' ? 1 : 0));
                        }
                        UIRenderer2D::DrawTextNative(d2d, L"  " + std::wstring(linhaDesc.begin(), linhaDesc.end()), rightX, rightY, D2D1::ColorF(0.7f, 0.7f, 0.7f), 16.0f, false);
                        rightY += 25.0f;
                    }
                }

                rightY += 20.0f;
                std::string passNome = classe->getNamePassivaClasse();
                if (!passNome.empty()) {
                    UIRenderer2D::DrawTextNative(d2d, L"[PASSIVA]", rightX, rightY, D2D1::ColorF(0.4f, 0.8f, 1.0f), 22.0f, false);
                    rightY += 35.0f;
                    UIRenderer2D::DrawTextNative(d2d, L"  " + std::wstring(passNome.begin(), passNome.end()), rightX, rightY, D2D1::ColorF(1.0f, 1.0f, 1.0f), 16.0f, false);
                    rightY += 30.0f;
                    
                    std::istringstream pStream(classe->obterDescricaoPassivaClasse());
                    while (std::getline(pStream, linhaDesc)) {
                        if (linhaDesc.empty()) continue;
                        while (linhaDesc.length() > 55) {
                            size_t brk = linhaDesc.rfind(' ', 55);
                            if (brk == std::string::npos) brk = 55;
                            { std::string part = linhaDesc.substr(0, brk);
                            UIRenderer2D::DrawTextNative(d2d, L"  " + std::wstring(part.begin(), part.end()), rightX, rightY, D2D1::ColorF(0.7f, 0.7f, 0.7f), 16.0f, false); }
                            rightY += 25.0f;
                            linhaDesc = linhaDesc.substr(brk + (linhaDesc[brk] == ' ' ? 1 : 0));
                        }
                        UIRenderer2D::DrawTextNative(d2d, L"  " + std::wstring(linhaDesc.begin(), linhaDesc.end()), rightX, rightY, D2D1::ColorF(0.7f, 0.7f, 0.7f), 16.0f, false);
                        rightY += 25.0f;
                    }
                }
            }

            UIRenderer2D::ResetTransform(d2d);
            rt->EndDraw();
        }

        // Processa a entrada capturada no inicio do laco
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
                TelaClasse::Resultado r;
                r.voltou = true;
                return r;
            }
            TelaClasse::Resultado r;
            r.indice = selecaoAtual;
            r.nome = opcoesGerais[selecaoAtual].nome;
            r.classeSelecionada = opcoesGerais[selecaoAtual].tipo;
            return r;
        } else if (tecla == 27) { // ESC = voltar
            TelaClasse::Resultado r;
            r.voltou = true;
            return r;
        }
    }
    
    TelaClasse::Resultado r_fallback;
    r_fallback.voltou = true;
    return r_fallback;
}
