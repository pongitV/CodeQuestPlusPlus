#include "ScreenMenuRaycaster.h"
#include "../utils/MenuRaycasterLayout.h"
#include "../utils/MenuRaycasterUtils.h"
#include <sstream>
#include <thread>
#include <chrono>
#include <vector>
#include <cmath>
#include "../../../../ui/screens/menu/ScreenMenuBase.h"
#include "../../../../core/utils/DialogFunctions.h"
#include "../../../../core/utils/InputControl.h"
#include "../../../../core/input/Win32Input.h"

#include "../../../../core/d2d-context/D2DContext.h"
#include "../../../../rendering/direct-2d/D2DRenderer.h"
#include "../../../../rendering/direct-2d/UIRenderer2D.h"
#include "../../../../rendering/direct-2d/GerenciadorTexturas2D.h"
#include "../../../../core/window/GameWindow.h"

int TelaMenuRaycaster::displayOpcoesMenuPrincipal() {
    InputControl::limparBuffer();
    MenuRaycasterUtils::cachearBackground3D("Village", nullptr);
    int selecaoAtual = 0;
    std::vector<std::string> opcoes = {"NOVO JOGO", "SAIR DO JOGO"};
    InputControl::limparBuffer();
    
    while (true) {
        if (auto* win = D2DContext::window) {
            win->processarMensagens();
        }
        InputControl::atualizarTeclas();

        // Entrada: leitura logo apos atualizarTeclas para capturar a borda de subida
        //    Antes que o passo de renderizacao de cerca de 16ms libere a tecla.
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

            UIDynamicBox box;
            float startY = (logicalH / 2.0f) - ((opcoes.size() * 50.0f) / 2.0f);

            for (int i = 0; i < (int)opcoes.size(); ++i) {
                std::wstring prefix = (i == selecaoAtual) ? L"> " : L"  ";
                std::string opcStr = opcoes[i];
                std::wstring wOpc(opcStr.begin(), opcStr.end());
                std::wstring text = prefix + wOpc;

                D2D1_COLOR_F corOpcao = (i == selecaoAtual) ? D2D1::ColorF(1.0f, 0.84f, 0.0f) : D2D1::ColorF(0.7f, 0.7f, 0.7f);
                box.AddText(text, logicalW / 2.0f, startY + i * 50.0f, 20.0f, corOpcao, true);
            }

            box.Render(d2d, D2D1::ColorF(0.0f, 0.0f, 0.0f), 0.85f, 2.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), 25.0f, logicalW / 2.0f);

            UIRenderer2D::ResetTransform(d2d);
            rt->EndDraw();
        }

        // Processa a entrada capturada no topo
        if (tecla == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60fps
            continue;
        }

        if (tecla == 'w' || tecla == 'W') {
            selecaoAtual = (selecaoAtual - 1 + (int)opcoes.size()) % (int)opcoes.size();
        } else if (tecla == 's' || tecla == 'S') {
            selecaoAtual = (selecaoAtual + 1) % (int)opcoes.size();
        } else if (tecla == '\r' || tecla == '\n') {
            if (selecaoAtual == 1) { // Sair
                if (displayConfirmacaoSaida()) {
                    return 1;
                }
            } else {
                return 0; // Novo Jogo
            }
        } else if (tecla == 27) { // ESC
            return 1;
        }
    }

    
    return 1;
}


namespace {
    inline std::wstring utf8_to_wstring(const std::string& str) {
        return MenuRaycasterUtils::toWStringClean(str);
    }
}

void TelaMenuRaycaster::displayPainelLogoJogo(const std::string& tituloDaTela, bool animarFadeIn) {
    MenuRaycasterUtils::cachearBackground3D("Village", nullptr);

    if (animarFadeIn) {
        for (int f = 0; f <= 15; ++f) {
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
                
                float alpha = f / 15.0f;
                std::wstring wTit = utf8_to_wstring(tituloDaTela);
                UIRenderer2D::DrawTextNative(d2d, wTit, cx, cy - 50.0f, D2D1::ColorF(1.0f, 20.0f, 1.0f, alpha), 22.0f, true);

                UIRenderer2D::ResetTransform(d2d);
                rt->EndDraw();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(32));
        }
    } else {
        auto d2d = D2DContext::renderer;
        if (d2d) {
            MenuRaycasterUtils::desenharFundoNativoD2D(true);
            auto rt = d2d->obterRenderTarget();
            if (rt) {
                auto tam = rt->GetSize();
                UIRenderer2D::SetupTransform(d2d, tam.width, tam.height);

                float cx = UIRenderer2D::LOGICAL_WIDTH / 2.0f;
                float cy = UIRenderer2D::LOGICAL_HEIGHT / 2.0f;
                
                std::wstring wTit = utf8_to_wstring(tituloDaTela);
                UIRenderer2D::DrawTextNative(d2d, wTit, cx, cy - 50.0f, D2D1::ColorF(1.0f, 0.84f, 0.0f, 1.0f), 22.0f, true);

                UIRenderer2D::ResetTransform(d2d);
                rt->EndDraw();
            }
        }
    }
}

bool TelaMenuRaycaster::displayConfirmacaoDeEscolhaComArteLadoALado(
    const std::string& tipoDeEscolha, const std::string& nomeDaEscolha,
    const std::vector<std::string>& informacoesParaExibir,
    const std::vector<std::string>& arteAsciiParaExibir)
{
    int selecaoAtual = 1;
    std::string pergunta = "CONFIRMAR " + tipoDeEscolha + "?";
    std::vector<std::string> opcoes = {"VOLTAR", "CONFIRMAR"};

    InputControl::limparBuffer();
    MenuRaycasterUtils::cachearBackground3D("Village", nullptr);

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

            std::string tituloStr = "PREVIA DA " + tipoDeEscolha + ": " + nomeDaEscolha;
            std::wstring titulo = utf8_to_wstring(tituloStr);

            UIRenderer2D::DrawTextNative(d2d, titulo, cx, 100.0f, D2D1::ColorF(1.0f, 0.84f, 0.0f), 22.0f, true);

            // Calcular larguras para posicionamento lado a lado
            float maxInfoW = 0.0f;
            for (const auto& info : informacoesParaExibir) {
                maxInfoW = std::max(maxInfoW, info.length() * 16.0f * 0.6f);
            }
            float maxArteW = 0.0f;
            for (const auto& arte : arteAsciiParaExibir) {
                maxArteW = std::max(maxArteW, (float)arte.length() * 16.0f * 0.6f);
            }
            float infoX = cx - (maxInfoW + maxArteW + 40.0f) / 2.0f;
            float arteX = infoX + maxInfoW + 40.0f;

            UIDynamicBox box;
            float infoY = 0.0f;
            float arteY = 0.0f;
            for (size_t i = 0; i < informacoesParaExibir.size(); ++i) {
                std::string plainInfo = informacoesParaExibir[i];
                std::wstring wInfo = utf8_to_wstring(plainInfo);
                
                D2D1_COLOR_F cor = D2D1::ColorF(1.0f, 1.0f, 1.0f);
                if (informacoesParaExibir[i].find("38;2;100;100;255") != std::string::npos) cor = D2D1::ColorF(0.4f, 0.4f, 1.0f);
                else if (informacoesParaExibir[i].find("38;2;0;255;255") != std::string::npos) cor = D2D1::ColorF(0.0f, 1.0f, 1.0f);
                else if (informacoesParaExibir[i].find("38;2;100;255;100") != std::string::npos) cor = D2D1::ColorF(0.4f, 1.0f, 0.4f);
                else if (informacoesParaExibir[i].find("38;2;255;80;80") != std::string::npos) cor = D2D1::ColorF(1.0f, 0.3f, 0.3f);
                else if (informacoesParaExibir[i].find("38;2;0;200;255") != std::string::npos) cor = D2D1::ColorF(0.0f, 0.8f, 1.0f);
                else if (informacoesParaExibir[i].find("38;2;180;180;180") != std::string::npos) cor = D2D1::ColorF(0.7f, 0.7f, 0.7f);

                box.AddText(wInfo, infoX, infoY + i * 30.0f, 16.0f, cor, false);
            }

            for (size_t i = 0; i < arteAsciiParaExibir.size(); ++i) {
                std::wstring wArte = utf8_to_wstring(arteAsciiParaExibir[i]);
                box.AddText(wArte, arteX, arteY + i * 18.0f, 16.0f, D2D1::ColorF(0.8f, 0.7f, 1.0f), false);
            }

            float opY = std::max(informacoesParaExibir.size() * 30.0f, arteAsciiParaExibir.size() * 18.0f) + 20.0f;
            for (int i = 0; i < (int)opcoes.size(); ++i) {
                std::wstring cursor = (i == selecaoAtual) ? L"> " : L"  ";
                std::wstring wOpcao = utf8_to_wstring(opcoes[i]);
                D2D1_COLOR_F corOpcao = (i == selecaoAtual) ? D2D1::ColorF(1.0f, 0.84f, 0.0f) : D2D1::ColorF(0.7f, 0.7f, 0.7f);
                box.AddText(cursor + wOpcao, cx - 150.0f + i * 300.0f, opY, 18.0f, corOpcao, true);
            }

            box.Render(d2d, D2D1::ColorF(0,0,0), 0.85f, 2.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), 20.0f, cx);

            UIRenderer2D::ResetTransform(d2d);
            rt->EndDraw();
        }

        if (tecla == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            continue;
        }

        if (tecla == 'a' || tecla == 'A' || tecla == 'w' || tecla == 'W') {
            selecaoAtual = (selecaoAtual - 1 + (int)opcoes.size()) % (int)opcoes.size();
        } else if (tecla == 'd' || tecla == 'D' || tecla == 's' || tecla == 'S') {
            selecaoAtual = (selecaoAtual + 1) % (int)opcoes.size();
        } else if (tecla == '\r' || tecla == '\n') {
            return selecaoAtual == 1;
        } else if (tecla == 27) {
            return false;
        }
    }
    return false;
}

std::vector<std::string> TelaMenuRaycaster::comporQuadroDeAtributos(
    const Attributes& stats, const std::string& tituloSecao,
    const std::string& tituloHabilidade, const std::string& nomeHab,
    const std::string& descHab,
    const std::string& tituloHabilidade2, const std::string& nomeHab2,
    const std::string& descHab2)
{
    auto formatarAtributo = [](const std::string& nomeAtr, int valorAtr) {
        std::string sinal = (valorAtr >= 0 ? "+" : "");
        return " - " + nomeAtr + ": " + sinal + std::to_string(valorAtr);
    };

    std::vector<std::string> resultado;
    resultado.push_back(tituloSecao);
    resultado.push_back(formatarAtributo("Health", stats.health));
    resultado.push_back(formatarAtributo("Forca", stats.strength));
    resultado.push_back(formatarAtributo("Destreza", stats.dexterity));
    resultado.push_back(formatarAtributo("Resistencia", stats.resistance));
    resultado.push_back(formatarAtributo("Constituicao", stats.constitution));
    resultado.push_back(formatarAtributo("Inteligencia", stats.intelligence));
    resultado.push_back(formatarAtributo("Sabedoria", stats.wisdom));
    resultado.push_back("");
    resultado.push_back(tituloHabilidade);
    resultado.push_back(" " + nomeHab);

    std::istringstream stream(descHab);
    std::string linha;
    while (std::getline(stream, linha)) {
        if (linha.empty()) continue;
        while (linha.length() > 75) {
            size_t brk = linha.rfind(' ', 75);
            if (brk == std::string::npos) brk = 75;
            resultado.push_back(" - " + linha.substr(0, brk));
            linha = linha.substr(brk + (linha[brk] == ' ' ? 1 : 0));
        }
        resultado.push_back(" - " + linha);
    }

    if (!tituloHabilidade2.empty()) {
        resultado.push_back("");
        resultado.push_back(tituloHabilidade2);
        resultado.push_back(" " + nomeHab2);
        std::istringstream stream2(descHab2);
        while (std::getline(stream2, linha)) {
            if (linha.empty()) continue;
            while (linha.length() > 75) {
                size_t brk = linha.rfind(' ', 75);
                if (brk == std::string::npos) brk = 75;
                resultado.push_back(" - " + linha.substr(0, brk));
                linha = linha.substr(brk + (linha[brk] == ' ' ? 1 : 0));
            }
            resultado.push_back(" - " + linha);
        }
    }

    return resultado;
}

bool TelaMenuRaycaster::displayConfirmacaoSaida() {
    int selecaoAtual = 1;
    std::string pergunta = "Deseja realmente sair do game?";
    std::vector<std::string> opcoes = {"NAO", "SIM"};
    
    InputControl::limparBuffer();
    MenuRaycasterUtils::cachearBackground3D("Village", nullptr);

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

            std::wstring wPergunta = utf8_to_wstring(pergunta);
            box.AddText(wPergunta, cx, cy - 40.0f, 22.0f, D2D1::ColorF(1.0f, 0.84f, 0.0f), true);

            float opY = cy + 30.0f;
            for (int i = 0; i < (int)opcoes.size(); ++i) {
                std::wstring cursor = (i == selecaoAtual) ? L"> " : L"  ";
                std::wstring wOpcao = utf8_to_wstring(opcoes[i]);
                D2D1_COLOR_F corOpcao = (i == selecaoAtual) ? D2D1::ColorF(1.0f, 0.84f, 0.0f) : D2D1::ColorF(0.7f, 0.7f, 0.7f);
                box.AddText(cursor + wOpcao, cx - 150.0f + i * 300.0f, opY, 18.0f, corOpcao, true);
            }

            box.Render(d2d, D2D1::ColorF(0.0f, 0.0f, 0.0f), 0.85f, 2.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), 25.0f, cx);

            UIRenderer2D::ResetTransform(d2d);
            rt->EndDraw();
        }

        if (tecla == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            continue;
        }

        if (tecla == 'a' || tecla == 'A' || tecla == 'w' || tecla == 'W') {
            selecaoAtual = (selecaoAtual - 1 + (int)opcoes.size()) % (int)opcoes.size();
        } else if (tecla == 'd' || tecla == 'D' || tecla == 's' || tecla == 'S') {
            selecaoAtual = (selecaoAtual + 1) % (int)opcoes.size();
        } else if (tecla == '\r' || tecla == '\n') {
            return selecaoAtual == 1;
        } else if (tecla == 27) {
            return false;
        }
    }
    return false;
}


