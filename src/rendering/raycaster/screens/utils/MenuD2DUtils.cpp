#include "MenuD2DUtils.h"

#include "../../../../core/d2d-context/D2DContext.h"
#include "../../../../core/window/GameWindow.h"
#include "../../../../rendering/direct-2d/D2DRenderer.h"
#include "../../../../rendering/direct-2d/UIRenderer2D.h"
#include "../../engine-raycaster/RaycasterWorld.h"
#include "../../engine-raycaster/Raycaster.h"
#include "../../engine-raycaster/RaycasterFrame.h"
#include "MenuRaycasterLayout.h"
#include "MenuRaycasterUtils.h"
#include "../../../../core/utils/InputControl.h"
#include "../../../../ui/UIManager.h"
#include <string>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>

namespace MenuRaycasterUtils {

float s_tempoMenuAnimacao = 30.0f;
bool s_mostrarLogoAbertura = true;

std::wstring utf8_to_wstring(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

size_t obterComprimentoVisivel(const std::string& str) {
    return str.length();
}

std::wstring toWStringClean(const std::string& str) {
    return utf8_to_wstring(str);
}

D2D1_COLOR_F converterCorParaD2D(Color cor) {
    switch (cor) {
        case Color::RED: case Color::RED_CLARO: return D2D1::ColorF(D2D1::ColorF::Red);
        case Color::GREEN: case Color::GREEN_CLARO: return D2D1::ColorF(D2D1::ColorF::Green);
        case Color::BLUE: case Color::BLUE_CLARO: return D2D1::ColorF(0.2f, 0.5f, 1.0f);
        case Color::YELLOW: case Color::YELLOW_CLARO: return D2D1::ColorF(D2D1::ColorF::Yellow);
        case Color::CYAN: case Color::CYAN_CLARO: return D2D1::ColorF(D2D1::ColorF::Cyan);
        case Color::MAGENTA: case Color::MAGENTA_CLARO: return D2D1::ColorF(D2D1::ColorF::Magenta);
        case Color::GRAY: return D2D1::ColorF(0.6f, 0.6f, 0.6f);
        case Color::BLACK: return D2D1::ColorF(D2D1::ColorF::Black);
        default: return D2D1::ColorF(D2D1::ColorF::White);
    }
}
static void atualizarFisicaCeleste(D2D1_SIZE_F tam, float dt) {
    POINT p;
    GetCursorPos(&p);
    if (D2DContext::window && D2DContext::window->obterHWND()) {
        ScreenToClient(D2DContext::window->obterHWND(), &p);
    }
    float mx = (float)p.x;
    float my = (float)p.y;
    bool mouseDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

    int cols = 192;
    int rows = 60;
    float cellH = std::min((tam.width / (float)cols) / 0.55f, tam.height / (float)rows);
    float cellW = cellH * 0.55f;
    float offsetX = (tam.width - (cellW * cols)) / 2.0f;
    float offsetY = (tam.height - (cellH * rows)) / 2.0f;
    float skyHeight = cellH * 30.0f;

    float t = s_tempoMenuAnimacao / 120.0f;

    // Calcula a posicao do sol na tela
    float sunAng = (t - 0.25f) * 3.2f;
    float sunColFrac = (sunAng / 1.6f) + 0.5f;
    float sunScreenX = offsetX + sunColFrac * (cols * cellW);
    float sunRatioY = 0.5f - 0.2f * std::cos((t - 0.25f) * 2.0f * 3.14159f);
    float sunScreenY = offsetY + sunRatioY * (rows * cellH);

    // Calcula a posicao da lua na tela
    float moonAng = (t - 0.75f) * 3.2f;
    float moonColFrac = (moonAng / 1.6f) + 0.5f;
    float moonScreenX = offsetX + moonColFrac * (cols * cellW);
    float moonRatioY = 0.5f - 0.2f * std::cos((t - 0.75f) * 2.0f * 3.14159f);
    float moonScreenY = offsetY + moonRatioY * (rows * cellH);

    static bool s_isDraggingSun = false;
    static bool s_isDraggingMoon = false;

    float clickRadius = std::max(70.0f, cellH * 6.0f);

    if (mouseDown) {
        if (!s_isDraggingSun && !s_isDraggingMoon) {
            float distSun = std::sqrt((mx - sunScreenX)*(mx - sunScreenX) + (my - sunScreenY)*(my - sunScreenY));
            float distMoon = std::sqrt((mx - moonScreenX)*(mx - moonScreenX) + (my - moonScreenY)*(my - moonScreenY));

            if (distSun < clickRadius && distSun <= distMoon) {
                s_isDraggingSun = true;
            } else if (distMoon < clickRadius) {
                s_isDraggingMoon = true;
            }
        }

        if (s_isDraggingSun) {
            float colFrac = (mx - offsetX) / (cols * cellW);
            float targetAng = (colFrac - 0.5f) * 1.6f;
            float newT = (targetAng / 3.2f) + 0.25f;
            while (newT < 0.0f) newT += 1.0f;
            while (newT >= 1.0f) newT -= 1.0f;
            s_tempoMenuAnimacao = newT * 120.0f;

            float mouseRatioY = (my - offsetY) / (rows * cellH);

            RaycasterWorld::s_overrideCelestials = true;
            RaycasterWorld::s_overrideSunAng = targetAng;
            RaycasterWorld::s_overrideSunRatioY = mouseRatioY;

            float moonAngNatural = (newT - 0.75f) * 3.2f;
            float moonRatioYNatural = 0.5f - 0.2f * std::cos((newT - 0.75f) * 2.0f * 3.14159f);
            RaycasterWorld::s_overrideMoonAng = moonAngNatural;
            RaycasterWorld::s_overrideMoonRatioY = moonRatioYNatural;
        } else if (s_isDraggingMoon) {
            float colFrac = (mx - offsetX) / (cols * cellW);
            float targetAng = (colFrac - 0.5f) * 1.6f;
            float newT = (targetAng / 3.2f) + 0.75f;
            while (newT < 0.0f) newT += 1.0f;
            while (newT >= 1.0f) newT -= 1.0f;
            s_tempoMenuAnimacao = newT * 120.0f;

            float mouseRatioY = (my - offsetY) / (rows * cellH);

            RaycasterWorld::s_overrideCelestials = true;
            RaycasterWorld::s_overrideMoonAng = targetAng;
            RaycasterWorld::s_overrideMoonRatioY = mouseRatioY;

            float sunAngNatural = (newT - 0.25f) * 3.2f;
            float sunRatioYNatural = 0.5f - 0.2f * std::cos((newT - 0.25f) * 2.0f * 3.14159f);
            RaycasterWorld::s_overrideSunAng = sunAngNatural;
            RaycasterWorld::s_overrideSunRatioY = sunRatioYNatural;
        }
    } else {
        s_isDraggingSun = false;
        s_isDraggingMoon = false;
        s_tempoMenuAnimacao += dt;
        if (s_tempoMenuAnimacao >= 120.0f) s_tempoMenuAnimacao -= 120.0f;
        RaycasterWorld::s_overrideCelestials = false;
    }
}

void desenharFundoNativoD2D(bool abrirFrame) {
    auto* d2d = D2DContext::renderer;
    auto* rt = d2d ? d2d->obterRenderTarget() : nullptr;
    if (!rt) return;

    if (abrirFrame) {
        rt->BeginDraw();
    }
    D2D1_SIZE_F tam = rt->GetSize();
    rt->Clear(D2D1::ColorF(0, 0, 0));

    static auto lastTime = std::chrono::steady_clock::now();
    auto nowTime = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(nowTime - lastTime).count();
    lastTime = nowTime;
    if (dt > 0.1f) dt = 0.1f;
    
    atualizarFisicaCeleste(tam, dt);

    int cols = 192;
    int rows = 60;
    float cellH = std::min((tam.width / (float)cols) / 0.55f, tam.height / (float)rows);
    float cellW = cellH * 0.55f;
    float offsetX = (tam.width - (cellW * cols)) / 2.0f;
    float offsetY = (tam.height - (cellH * rows)) / 2.0f;

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            float colAng = ((float)x / (float)cols - 0.5f) * 1.6f;
            Pixel3D pxCeu = RaycasterWorld::obterPixelTeto(0, colAng, colAng, y, rows * 2, s_tempoMenuAnimacao, true);
            d2d->preencherRetangulo(offsetX + x * cellW, offsetY + y * cellH, cellW + 0.5f, cellH + 0.5f, D2D1::ColorF(pxCeu.r / 255.0f, pxCeu.g / 255.0f, pxCeu.b / 255.0f));
            if (pxCeu.ch != ' ') {
                d2d->preencherRetangulo(offsetX + x * cellW + cellW*0.25f, offsetY + y * cellH + cellH*0.25f, cellW*0.5f, cellH*0.5f, D2D1::ColorF(pxCeu.fgR / 255.0f, pxCeu.fgG / 255.0f, pxCeu.fgB / 255.0f));
            }
        }
    }
}

int renderizarMenuPadrao(
    const std::wstring& titulo,
    D2D1_COLOR_F corTema,
    const std::vector<std::string>& opcoes,
    const std::vector<std::string>& arte,
    const std::vector<GrupoCorUI>& paletaArte,
    PosicaoArte posicaoArte,
    float escalaArte,
    std::function<void(UIDynamicBox&, int, float, float, float)> construtorAdicional,
    std::function<bool(char, int&)> handlerEntradaExtra,
    const std::vector<std::string>& tituloAscii,
    const std::vector<GrupoCorUI>& paletaAscii,
    const std::vector<std::string>& textoContexto
) {
    auto construtorCaixa = [&](UIDynamicBox& box, int selecaoAtual, float logicalW, float logicalH) {
        if (!tituloAscii.empty()) {
            box.SetTitle(L"", corTema);
            box.SetTitleAscii(tituloAscii, paletaAscii, 3.0f);
        } else if (!titulo.empty()) {
            box.SetTitle(titulo, corTema);
        }

        float pixelScale = escalaArte;
        float arteW = 0.0f;
        float arteH = 0.0f;
        if (!arte.empty() && posicaoArte != PosicaoArte::NENHUMA) {
            float maxW = 0;
            for (const auto& linha : arte) {
                std::string temp = linha;
                while (!temp.empty() && (temp.back() == ' ' || temp.back() == '\r')) {
                    temp.pop_back();
                }
                if (temp.size() > maxW) maxW = temp.size();
            }
            arteW = maxW * pixelScale;
            arteH = arte.size() * pixelScale;
        }

        float contentStartY = 100.0f;
        float opcoesStartCol = logicalW / 2.0f;
        float menuTotalW = 400.0f; // Largura aproximada para as opcoes
        float artX = 0.0f;
        float artY = 0.0f;

        if (posicaoArte == PosicaoArte::ESQUERDA) {
            float totalW = arteW + 50.0f + menuTotalW;
            float startX = (logicalW - totalW) / 2.0f;
            artX = startX;
            artY = contentStartY;
            opcoesStartCol = startX + arteW + 50.0f + menuTotalW / 2.0f;
            
            // Calcula a altura do bloco de texto para centralizar verticalmente
            float blockH = 0.0f;
            if (!textoContexto.empty()) blockH += textoContexto.size() * 25.0f + 20.0f;
            blockH += opcoes.size() * 40.0f;
            
            if (arteH > blockH) {
                contentStartY = artY + (arteH - blockH) / 2.0f;
            } else if (blockH > arteH) {
                artY = contentStartY + (blockH - arteH) / 2.0f;
            }
            
            box.AddPixelArt(arte, paletaArte, artX, artY, pixelScale);
        } else if (posicaoArte == PosicaoArte::DIREITA) {
            float totalW = menuTotalW + 50.0f + arteW;
            float startX = (logicalW - totalW) / 2.0f;
            opcoesStartCol = startX + menuTotalW / 2.0f;
            artX = startX + menuTotalW + 50.0f;
            artY = contentStartY;
            box.AddPixelArt(arte, paletaArte, artX, artY, pixelScale);
        } else if (posicaoArte == PosicaoArte::ACIMA) {
            artX = (logicalW - arteW) / 2.0f;
            artY = 60.0f;
            contentStartY = artY + arteH + 40.0f;
            box.AddPixelArt(arte, paletaArte, artX, artY, pixelScale);
        } else if (posicaoArte == PosicaoArte::ABAIXO) {
            float opsH = opcoes.size() * 50.0f;
            artX = (logicalW - arteW) / 2.0f;
            artY = contentStartY + opsH + 40.0f;
            box.AddPixelArt(arte, paletaArte, artX, artY, pixelScale);
        }

        if (construtorAdicional) {
            construtorAdicional(box, selecaoAtual, opcoesStartCol, contentStartY, pixelScale);
        } else {
            float opY = contentStartY;
            
            // Desenha texto de contexto
            if (!textoContexto.empty()) {
                for (const auto& linha : textoContexto) {
                    std::wstring wLinha = MenuRaycasterUtils::utf8_to_wstring(linha);
                    box.AddText(wLinha, opcoesStartCol, opY, 16.0f, D2D1::ColorF(D2D1::ColorF::White), true);
                    opY += 25.0f;
                }
                opY += 20.0f; // Espacamento adicional entre texto e opcoes
            }
            
            // Desenha opcoes se nenhum construtor personalizado for fornecido
            for (int i = 0; i < (int)opcoes.size(); ++i) {
                std::string opcStr = opcoes[i];
                std::wstring wOpc(opcStr.begin(), opcStr.end());
                D2D1_COLOR_F corOpcao = (i == selecaoAtual) ? corTema : D2D1::ColorF(0.6f, 0.6f, 0.6f);
                MenuRaycasterUtils::adicionarOpcaoMenu(box, wOpc, opcoesStartCol, opY, (i == selecaoAtual), corOpcao, true);
                opY += 40.0f;
            }
        }
    };

    return renderizarPopupCaixa({}, {}, opcoes.size(), construtorCaixa, handlerEntradaExtra);
}

int renderizarPopupCaixa(
    const std::vector<std::string>& tituloAscii,
    const std::vector<GrupoCorUI>& paletaAscii,
    int numOpcoes,
    std::function<void(UIDynamicBox&, int, float, float)> construtorCaixa,
    std::function<bool(char, int&)> handlerEntradaExtra
) {
    int selecaoAtual = 0;
    bool aguardandoSoltarESC = ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0);
    ClipCursor(nullptr);
    GameWindow::mostrarCursor();
    InputControl::limparBuffer();
    
    while (true) {
        if (auto* win = D2DContext::window) {
            win->processarMensagens();
        }
        InputControl::atualizarTeclas();
        char tecla = InputControl::lerTecla();

        if (aguardandoSoltarESC) {
            if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) == 0) {
                aguardandoSoltarESC = false;
            } else if (tecla == 27) {
                tecla = 0;
            }
        }

        auto d2d = D2DContext::renderer;
        if (!d2d) break;

        incrementarCicloDia();

        auto rt = d2d->obterRenderTarget();
        if (rt) {
            rt->BeginDraw();
            if (ID2D1Bitmap* tex = d2d->obterTexturaBackbuffer()) {
                D2D1_SIZE_U tsz = tex->GetPixelSize();
                D2D1_SIZE_F rsz = rt->GetSize();
                rt->DrawBitmap(tex, D2D1::RectF(0, 0, rsz.width, rsz.height), 0.50f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(0, 0, (FLOAT)tsz.width, (FLOAT)tsz.height));
                d2d->preencherRetangulo(0, 0, rsz.width, rsz.height, D2D1::ColorF(0.02f, 0.02f, 0.05f, 0.40f));
            }
        }

        if (rt) {

            auto tam = rt->GetSize();
            UIRenderer2D::SetupTransform(d2d, tam.width, tam.height);


            float logicalW = UIRenderer2D::LOGICAL_WIDTH;
            float logicalH = UIRenderer2D::LOGICAL_HEIGHT;

            // Renderiza arte do titulo
            if (!tituloAscii.empty()) {
                float baseScale = 8.0f;
                // Animacao de pulsar (aproximacao e afastamento)
                float scaleMultiplier = 1.0f + std::sin(s_tempoMenuAnimacao * 4.0f) * 0.1f;
                float pixelScale = baseScale * scaleMultiplier;
                
                int maxChars = 0;
                for (const auto& s : tituloAscii) {
                    if ((int)s.size() > maxChars) maxChars = s.size();
                }
                float asciiW = maxChars * pixelScale;
                float asciiX = (logicalW - asciiW) / 2.0f;
                
                // Mantem o centro do titulo estavel durante a escala
                float baseHeight = tituloAscii.size() * baseScale;
                float currentHeight = tituloAscii.size() * pixelScale;
                float asciiY = 50.0f - (currentHeight - baseHeight) / 2.0f;
                
                UIRenderer2D::DrawPixelArt(d2d, tituloAscii, paletaAscii, asciiX, asciiY, pixelScale, 1.0f);
            }

            UIDynamicBox box;
            if (construtorCaixa) {
                construtorCaixa(box, selecaoAtual, logicalW, logicalH);
            }

            box.Render(d2d, D2D1::ColorF(0.05f, 0.05f, 0.08f, 0.95f), 0.9f, 2.0f, D2D1::ColorF(0.5f, 0.5f, 0.5f), 20.0f, logicalW / 2.0f, logicalH / 2.0f);

            UIRenderer2D::ResetTransform(d2d);
            rt->EndDraw();
        }

        if (tecla == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            continue;
        }

        if (handlerEntradaExtra && handlerEntradaExtra(tecla, selecaoAtual)) {
            continue; // Manipulador adicional tratou a tecla
        }
        
        if (numOpcoes > 0) {
            if (tecla == 'w' || tecla == 'W') {
                selecaoAtual = (selecaoAtual - 1 + numOpcoes) % numOpcoes;
            } else if (tecla == 's' || tecla == 'S') {
                selecaoAtual = (selecaoAtual + 1) % numOpcoes;
            } else if (tecla == '\r' || tecla == '\n' || tecla == ' ') {
                return selecaoAtual;
            }
        }
        
        if (tecla == 27) { // ESC
            return -1;
        }
    }
    return -1;
}

void adicionarOpcaoMenu(UIDynamicBox& box, const std::wstring& texto, float x, float y, bool selecionado, D2D1_COLOR_F corBase, bool centralizado) {
    // Se centralizado, desloca levemente para a esquerda para compensar a ilusao de otica do prefixo "> "
    float drawX = centralizado ? (x - 12.0f) : x;

    if (selecionado) {
        float pulse = (sin(GetTickCount64() * 0.005f) + 1.0f) * 0.5f; // 0.0 to 1.0
        float pulseG = 0.5f + pulse * 0.5f; // 0.5 to 1.0
        D2D1_COLOR_F corSel = D2D1::ColorF(0.2f, pulseG, 0.2f); // Pulsing green
        
        box.AddText(L">" + texto, drawX, y, 18.0f, corSel, centralizado);
    } else {
        box.AddText(L" " + texto, drawX, y, 18.0f, corBase, centralizado);
    }
}

std::string lerEntradaTextoD2D(const std::wstring& prompt, int maxLength) {
    std::string inputStr = "";
    ClipCursor(nullptr);
    GameWindow::mostrarCursor();
    InputControl::limparBuffer();

    while (true) {
        if (auto* win = D2DContext::window) {
            win->processarMensagens();
        }
        InputControl::atualizarTeclas();
        char tecla = InputControl::lerTecla();

        auto d2d = D2DContext::renderer;
        if (!d2d) break;

        incrementarCicloDia();

        if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
            auto rt = d2d->obterRenderTarget();
            if (rt) {
                rt->BeginDraw();
                rt->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
                auto tex = d2d->obterTexturaBackbuffer();
                if (tex) {
                    D2D1_SIZE_U tsz = tex->GetPixelSize();
                    D2D1_SIZE_F rsz = rt->GetSize();
                    rt->DrawBitmap(tex, D2D1::RectF(0, 0, rsz.width, rsz.height), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(0, 0, (FLOAT)tsz.width, (FLOAT)tsz.height));
                }
                RaycasterFrame::restaurarUltimoQuadro();
            }
        } else {
            desenharFundoNativoD2D(true);
        }

        auto rt = d2d->obterRenderTarget();
        if (rt) {
            auto tam = rt->GetSize();
            UIRenderer2D::SetupTransform(d2d, tam.width, tam.height);

            float logicalW = UIRenderer2D::LOGICAL_WIDTH;
            float logicalH = UIRenderer2D::LOGICAL_HEIGHT;

            UIDynamicBox box;
            box.SetTitle(prompt, D2D1::ColorF(1.0f, 0.84f, 0.0f));
            
            std::string displayStr = inputStr;
            if ((GetTickCount64() / 500) % 2 == 0) {
                displayStr += "_";
            }
            box.AddText(utf8_to_wstring(displayStr), logicalW / 2.0f, logicalH / 2.0f, 22.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), true);

            box.Render(d2d, D2D1::ColorF(0.05f, 0.05f, 0.08f, 0.95f), 0.9f, 2.0f, D2D1::ColorF(0.5f, 0.5f, 0.5f), 20.0f, logicalW / 2.0f, logicalH / 2.0f);

            UIRenderer2D::ResetTransform(d2d);
            rt->EndDraw();
        }

        if (tecla == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            continue;
        }

        if (tecla == '\r' || tecla == '\n') {
            return inputStr;
        } else if (tecla == 27) { // ESC
            return "";
        } else if (tecla == 8) { // Backspace
            if (!inputStr.empty()) {
                inputStr.pop_back();
            }
        } else if (tecla >= 32 && tecla <= 126 && (int)inputStr.length() < maxLength) {
            inputStr += tecla;
        }
    }
    return inputStr;
}

}
