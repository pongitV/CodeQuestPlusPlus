#include "ScreenOpeningRaycaster.h"
#include "../utils/MenuRaycasterLayout.h"
#include "../utils/MenuRaycasterUtils.h"
#include <fstream>
#include <vector>
#include <cstdint>
#include <sstream>
#include <thread>
#include <chrono>

#include "../../../../ui/screens/menu/ScreenMenuBase.h"
#include "../../../../core/utils/InputControl.h"
#include "../../../../rendering/direct-2d/UIRenderer2D.h"
#include "../../../../core/d2d-context/D2DContext.h"
#include "../../../../rendering/direct-2d/D2DRenderer.h"
#include "../../../../core/window/GameWindow.h"

void TelaAberturaRaycaster::display() {
    MenuRaycasterUtils::s_mostrarLogoAbertura = true;
    MenuRaycasterUtils::cachearBackground3D("Village", nullptr);

    InputControl::limparBuffer();
    while (!InputControl::teclaPressionada()) {
        if (auto* win = D2DContext::window) {
            win->processarMensagens();
        }
        InputControl::atualizarTeclas();
        
        auto d2d = D2DContext::renderer;
        if (!d2d) break;

        // 1. Draw Native Sky
        MenuRaycasterUtils::desenharFundoNativoD2D(/*abrirFrame=*/true);

        // 2. Setup Transform for 1920x1080 UI Space
        auto rt = d2d->obterRenderTarget();
        if (rt) {
            auto tam = rt->GetSize();
            UIRenderer2D::SetupTransform(d2d, tam.width, tam.height);

            // Calculate Logical Y scaling mapping
            float logicalW = UIRenderer2D::LOGICAL_WIDTH;
            float logicalH = UIRenderer2D::LOGICAL_HEIGHT;

            // Pixel size of the logo blocks
            float pixelScale = 8.0f;
            
            auto countUtf8Chars = [](const std::string& str) {
                int count = 0;
                for (size_t lx = 0; lx < str.size(); ) {
                    unsigned char c = str[lx];
                    int charLen = 1;
                    if ((c & 0x80) == 0) charLen = 1;
                    else if ((c & 0xE0) == 0xC0) charLen = 2;
                    else if ((c & 0xF0) == 0xE0) charLen = 3;
                    else if ((c & 0xF8) == 0xF0) charLen = 4;
                    lx += charLen;
                    count++;
                }
                return count;
            };

            float time = MenuRaycasterUtils::s_tempoMenuAnimacao;
            float scaleMultiplier = 1.0f + sin(time * 3.0f) * 0.05f; 
            float currentPixelScale = pixelScale * scaleMultiplier;

            int logoCharWidth = countUtf8Chars(ArtesRaycaster::logoTexto[0]);
            float logoPixelWidth = logoCharWidth * currentPixelScale;
            
            float plusPixelWidth = 0.0f;
            float espacamentoPlus = 10.0f * scaleMultiplier;
            if (!ArtesRaycaster::logoPlus.empty()) {
                int plusCharWidth = countUtf8Chars(ArtesRaycaster::logoPlus[0]);
                plusPixelWidth = plusCharWidth * currentPixelScale;
            } else {
                espacamentoPlus = 0.0f;
            }
            
            float totalWidth = logoPixelWidth + espacamentoPlus + plusPixelWidth;
            float startX = (logicalW - totalWidth) / 2.0f;
            
            float baseLogoHeight = ArtesRaycaster::logoTexto.size() * (pixelScale * 1.5f);
            float currentLogoHeight = baseLogoHeight * scaleMultiplier;
            float startY = logicalH * 0.1f - (currentLogoHeight - baseLogoHeight) / 2.0f;

            // Prepare palette
            std::vector<GrupoCorUI> paletaLogo = {
                {"█", 255, 255, 255}
            };
            std::vector<GrupoCorUI> paletaPlus = {
                {"█", 255, 140, 0}
            };

            // Draw Logo CodeQuest
            UIRenderer2D::DrawPixelArt(d2d, ArtesRaycaster::logoTexto, paletaLogo, startX, startY, currentPixelScale, 1.0f);
            
            // Draw Plus
            if (!ArtesRaycaster::logoPlus.empty()) {
                UIRenderer2D::DrawPixelArt(d2d, ArtesRaycaster::logoPlus, paletaPlus, startX + logoPixelWidth + espacamentoPlus, startY, currentPixelScale, 1.0f);
            }

            float logoHeight = ArtesRaycaster::logoTexto.size() * (pixelScale * 1.5f);
            UIRenderer2D::DrawTextNative(d2d, L"Pressione qualquer tecla para continuar...", logicalW / 2.0f, startY + logoHeight + 100.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f), 18.0f, true);

            // Draw Version
            UIRenderer2D::DrawTextNative(d2d, L"Versão 0.1", logicalW - 200.0f, logicalH - 40.0f, D2D1::ColorF(0.5f, 0.5f, 0.5f), 24.0f, false);

            UIRenderer2D::ResetTransform(d2d);
            rt->EndDraw();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60fps
    }

    InputControl::lerTecla();
    InputControl::limparBuffer();
    MenuRaycasterUtils::s_mostrarLogoAbertura = false;
}
