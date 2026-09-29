#include "ScreenDifficultyRaycaster.h"
#include "../utils/MenuRaycasterLayout.h"
#include "../utils/MenuRaycasterUtils.h"
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>

#include "../../../../ui/screens/menu/ScreenMenuBase.h"
#include "../../../../core/utils/DialogFunctions.h"
#include "../../../../core/utils/InputControl.h"

#include "../../../../rendering/direct-2d/UIRenderer2D.h"
#include "../../../../core/d2d-context/D2DContext.h"
#include "../../../../core/window/GameWindow.h"
#include <windows.h>

static const std::vector<std::string> arteCaveira = {
    "⠀⠀⠀⠀⣠⣤⣶⣶⣶⣤⣄⡀⠀",
    "⠀⠀⣴⣾⣿⣿⣿⣿⣿⣧⡀⠈⠢",
    "⠀⣼⣿⣿⣿⣿⣿⣿⣿⡿⠁⠀⠀",
    "⢰⡿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀",
    "⠘⣽⡿⠿⠿⣿⣿⣿⣿⣿⣦⣤⡀",
    "⠀⣟⠀⠀⠀⣸⣿⡏⠀⠀⠀⢹⠗",
    "⠀⣿⣷⣶⣾⡿⠁⠙⣄⣀⣀⣠⡀",
    "⠀⠙⠙⢿⡿⣷⣶⣤⣿⣿⡿⠿⠃",
    "⠀⠀⠀⠺⡏⡏⡏⡏⡏⠉⠁⠀⠀",
    "⠀⠀⠀⠀⠀⠀⠁⠁⠀⠀⠀⠀⠀",
};

TelaDificuldade::Resultado TelaDificuldadeRaycaster::display(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse) {
    std::vector<std::string> opcoes = {
        "FACIL   (1.0x Attributes enemies)",
        "MEDIO   (1.5x Attributes enemies)",
        "DIFICIL (2.0x Attributes enemies)",
        "VOLTAR"
    };

    struct CorDificuldade { int r, g, b; };
    std::vector<CorDificuldade> cores = {
        {100, 255, 100},
        {255, 215, 0},
        {255, 80, 80},
        {180, 180, 180}
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
            float opX = logicalW / 2.0f - 200.0f;
            float opY = 240.0f;

            for (int i = 0; i < (int)opcoes.size(); ++i) {
                std::string opStr = (i == selecaoAtual ? "> " : "  ") + opcoes[i];
                std::wstring wOpStr(opStr.begin(), opStr.end());
                
                D2D1_COLOR_F corText = (i == selecaoAtual) ? D2D1::ColorF(cores[i].r/255.0f, cores[i].g/255.0f, cores[i].b/255.0f) : D2D1::ColorF(0.5f, 0.5f, 0.5f);
                mainBox.AddText(wOpStr, opX, opY + i * 60.0f, 18.0f, corText, false);
            }

            // Arte de caveira dentro da caixa, posicionada a direita do texto
            float arteX = opX + 350.0f;
            float arteY = opY;
            D2D1_COLOR_F arteColor = D2D1::ColorF(cores[selecaoAtual].r/255.0f, cores[selecaoAtual].g/255.0f, cores[selecaoAtual].b/255.0f);
            
            for (size_t i = 0; i < arteCaveira.size(); ++i) {
                int wchars_num = MultiByteToWideChar(CP_UTF8, 0, arteCaveira[i].c_str(), -1, NULL, 0);
                if (wchars_num > 0) {
                    std::wstring wArte(wchars_num, 0);
                    MultiByteToWideChar(CP_UTF8, 0, arteCaveira[i].c_str(), -1, &wArte[0], wchars_num);
                    if (!wArte.empty() && wArte.back() == L'\0') wArte.pop_back();
                    float skullWidth = (float)wArte.length() * 16.0f * 0.8f;
                    mainBox.AddText(wArte, arteX, arteY + i * 24.0f, 16.0f, arteColor, false, true, skullWidth);
                }
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
                TelaDificuldade::Resultado r;
                r.voltou = true;
                return r;
            }
            TelaDificuldade::Resultado r;
            r.indice = selecaoAtual;
            return r;
        } else if (tecla == 27) { // ESC = voltar
            TelaDificuldade::Resultado r;
            r.voltou = true;
            return r;
        }
    }
    
    TelaDificuldade::Resultado r_fallback;
    r_fallback.voltou = true;
    return r_fallback;
}


