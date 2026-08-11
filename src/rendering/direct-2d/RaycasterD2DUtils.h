#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>

#include "../raycaster/engine-raycaster/RaycasterSprites.h"

class D2DRenderer;

namespace RaycasterD2DUtils {
    inline void preencherBackbuffer(D2DRenderer& d2d,
        const std::vector<Pixel3D>& tela3D,
        int LARGURA_TELA, int ALTURA_TELA)
    {
        uint32_t* bb = d2d.obterBackbuffer();
        int bbW = D2DRenderer::BACKBUFFER_WIDTH;
        int bbH = D2DRenderer::BACKBUFFER_HEIGHT;

        if (LARGURA_TELA == bbW && ALTURA_TELA == bbH) {
            const Pixel3D* src = tela3D.data();
            int totalPixels = bbW * bbH;
            for (int i = 0; i < totalPixels; i++) {
                bb[i] = 0xFF000000 | ((uint32_t)src[i].r << 16) | ((uint32_t)src[i].g << 8) | (uint32_t)src[i].b;
            }
        } else {
            int copiaW = std::min(LARGURA_TELA, bbW);
            int copiaH = std::min(ALTURA_TELA, bbH);
            for (int y = 0; y < copiaH; y++) {
                const Pixel3D* srcRow = &tela3D[y * LARGURA_TELA];
                uint32_t* dstRow = &bb[y * bbW];
                for (int x = 0; x < copiaW; x++) {
                    dstRow[x] = 0xFF000000 | ((uint32_t)srcRow[x].r << 16) | ((uint32_t)srcRow[x].g << 8) | (uint32_t)srcRow[x].b;
                }
            }
        }
    }

    inline void desenharBitmapTelaCheia(D2DRenderer& d2d, ID2D1Bitmap* bitmap, float opacity = 1.0f) {
        if (!bitmap || !d2d.obterRenderTarget()) return;
        D2D1_SIZE_U tsz = bitmap->GetPixelSize();
        D2D1_SIZE_F rsz = d2d.obterRenderTarget()->GetSize();
        d2d.obterRenderTarget()->DrawBitmap(bitmap,
            D2D1::RectF(0, 0, rsz.width, rsz.height), opacity,
            D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
            D2D1::RectF(0, 0, (FLOAT)tsz.width, (FLOAT)tsz.height));
    }
}
