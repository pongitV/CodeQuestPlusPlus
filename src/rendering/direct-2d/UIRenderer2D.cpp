#include "UIRenderer2D.h"
#include <algorithm>
#include <cmath>
#include <string>

void UIRenderer2D::DrawBox(D2DRenderer* d2d, float x, float y, float w, float h, D2D1_COLOR_F corBase, float opacidade, float espessuraBorda, D2D1_COLOR_F corBorda) {
    if (!d2d || !d2d->obterRenderTarget()) return;
    
    corBase.a = opacidade;
    d2d->preencherRetangulo(x, y, w, h, corBase);
    
    if (espessuraBorda > 0.0f) {
        corBorda.a = opacidade;
        d2d->desenharRetangulo(x, y, w, h, corBorda, espessuraBorda);
    }
}

void UIRenderer2D::DrawPixelArt(D2DRenderer* d2d, const std::vector<std::string>& arte, const std::vector<GrupoCorUI>& paleta, float x, float y, float pixelScale, float opacidade, float pixelScaleY) {
    if (!d2d || !d2d->obterRenderTarget()) return;

    if (pixelScaleY < 0.0f) {
        pixelScaleY = pixelScale * 1.5f;
    }

    for (size_t ly = 0; ly < arte.size(); ++ly) {
        std::string linha = arte[ly];
        int logicalX = 0;
        for (size_t lx = 0; lx < linha.size(); ) {
            unsigned char c = linha[lx];
            int charLen = 1;
            if ((c & 0x80) == 0) charLen = 1;
            else if ((c & 0xE0) == 0xC0) charLen = 2;
            else if ((c & 0xF0) == 0xE0) charLen = 3;
            else if ((c & 0xF8) == 0xF0) charLen = 4;
            
            std::string utf8char = linha.substr(lx, charLen);
            lx += charLen;

            if (utf8char == " ") { logicalX++; continue; }

            D2D1_COLOR_F cor = D2D1::ColorF(1, 1, 1, opacidade);
            bool achouColor = false;

            for (const auto& g : paleta) {
                if (g.chars == "*" || g.chars.find(utf8char) != std::string::npos) {
                    cor = D2D1::ColorF(g.r / 255.0f, g.g / 255.0f, g.b / 255.0f, opacidade);
                    achouColor = true;
                    break;
                }
            }
            
            if (achouColor) {
                d2d->preencherRetangulo(x + logicalX * pixelScale, y + ly * pixelScaleY, pixelScale + 0.5f, pixelScaleY + 0.5f, cor);
            }
            logicalX++;
        }
    }
}

void UIRenderer2D::DrawPixelArtWave(D2DRenderer* d2d, const std::vector<std::string>& arte, const std::vector<GrupoCorUI>& paleta, float x, float y, float pixelScale, float time, float amplitude, float frequency, float opacidade, float pixelScaleY) {
    if (!d2d || !d2d->obterRenderTarget()) return;

    if (pixelScaleY < 0.0f) {
        pixelScaleY = pixelScale * 1.5f;
    }

    for (size_t ly = 0; ly < arte.size(); ++ly) {
        std::string linha = arte[ly];
        int logicalX = 0;
        for (size_t lx = 0; lx < linha.size(); ) {
            unsigned char c = linha[lx];
            int charLen = 1;
            if ((c & 0x80) == 0) charLen = 1;
            else if ((c & 0xE0) == 0xC0) charLen = 2;
            else if ((c & 0xF0) == 0xE0) charLen = 3;
            else if ((c & 0xF8) == 0xF0) charLen = 4;
            
            std::string utf8char = linha.substr(lx, charLen);
            lx += charLen;

            if (utf8char == " ") { logicalX++; continue; }

            D2D1_COLOR_F cor = D2D1::ColorF(1, 1, 1, opacidade);
            bool achouColor = false;

            for (const auto& g : paleta) {
                if (g.chars == "*ALL*" || g.chars.find(utf8char) != std::string::npos) {
                    cor = D2D1::ColorF(g.r / 255.0f, g.g / 255.0f, g.b / 255.0f, opacidade);
                    achouColor = true;
                    break;
                }
            }
            
            if (achouColor) {
                float waveOffset = sin(time + logicalX * frequency) * amplitude;
                d2d->preencherRetangulo(x + logicalX * pixelScale, y + ly * pixelScaleY + waveOffset, pixelScale + 0.5f, pixelScaleY + 0.5f, cor);
            }
            logicalX++;
        }
    }
}

void UIRenderer2D::DrawTextNative(D2DRenderer* d2d, const std::wstring& texto, float x, float y, D2D1_COLOR_F cor, float tamanhoFonte, bool centralizado, float maxWidth) {
    if (!d2d || !d2d->obterRenderTarget()) return;

    if (centralizado) {
        d2d->desenharTextoCentralizado(texto, x - maxWidth / 2.0f, y, maxWidth, tamanhoFonte, cor, tamanhoFonte);
    } else {
        d2d->desenharTexto(texto, x, y, cor, tamanhoFonte);
    }
}

void UIRenderer2D::SetupTransform(D2DRenderer* d2d, float windowWidth, float windowHeight) {
    if (!d2d || !d2d->obterRenderTarget()) return;
    
    float scaleX = (float)windowWidth / LOGICAL_WIDTH;
    float scaleY = (float)windowHeight / LOGICAL_HEIGHT;
    
    d2d->obterRenderTarget()->SetTransform(D2D1::Matrix3x2F::Scale(scaleX, scaleY));
}

void UIRenderer2D::ResetTransform(D2DRenderer* d2d) {
    if (!d2d || !d2d->obterRenderTarget()) return;
    d2d->obterRenderTarget()->SetTransform(D2D1::Matrix3x2F::Identity());
}

void UIDynamicBox::updateBounds(float x1, float y1, float x2, float y2) {
    if (x1 < minX) minX = x1;
    if (y1 < minY) minY = y1;
    if (x2 > maxX) maxX = x2;
    if (y2 > maxY) maxY = y2;
}

void UIDynamicBox::AddText(const std::wstring& text, float x, float y, float size, D2D1_COLOR_F color, bool center, bool visible, float customWidth) {
    texts.push_back({text, x, y, size, color, center, visible});
    
    float width = (customWidth > 0.0f) ? customWidth : (float)text.length() * size * 0.6f;
    float height = size * 1.2f;

    if (center) {
        updateBounds(x - width / 2.0f, y - height * 0.1f, x + width / 2.0f, y + height);
    } else {
        updateBounds(x, y - height * 0.1f, x + width, y + height);
    }
}

void UIDynamicBox::AddRect(float x, float y, float w, float h, D2D1_COLOR_F cor) {
    rects.push_back({x, y, w, h, cor});
    updateBounds(x, y, x + w, y + h);
}

void UIDynamicBox::AddPixelArt(const std::vector<std::string>& arte, const std::vector<GrupoCorUI>& paleta, float x, float y, float scale, float opacity) {
    arts.push_back({arte, paleta, x, y, scale, opacity});
    
    if (arte.empty()) return;
    
    float width = 0;
    if (!arte.empty()) {
        float maxW = 0;
        for (const auto& linha : arte) {
            std::string temp = linha;
            while (!temp.empty() && (temp.back() == ' ' || temp.back() == '\r')) {
                temp.pop_back();
            }
            if (temp.size() > maxW) maxW = temp.size();
        }
        width = maxW * scale;
    }
    float height = arte.size() * (scale * 1.5f);
    
    updateBounds(x, y, x + width, y + height);
}

void UIDynamicBox::SetTitle(const std::wstring& titulo, D2D1_COLOR_F cor) {
    m_titulo = titulo;
    m_corTitulo = cor;
}

void UIDynamicBox::SetTitleAscii(const std::vector<std::string>& tituloAscii, const std::vector<GrupoCorUI>& paleta, float scale) {
    m_tituloAscii = tituloAscii;
    m_paletaTituloAscii = paleta;
    m_tituloAsciiScale = scale;
    if (m_paletaTituloAscii.empty() && !m_tituloAscii.empty()) {
        int r = static_cast<int>(m_corTitulo.r * 255);
        int g = static_cast<int>(m_corTitulo.g * 255);
        int b = static_cast<int>(m_corTitulo.b * 255);
        m_paletaTituloAscii.push_back({"█", r, g, b});
        m_paletaTituloAscii.push_back({"▓", static_cast<int>(r * 0.8f), static_cast<int>(g * 0.8f), static_cast<int>(b * 0.8f)});
        m_paletaTituloAscii.push_back({"▒", static_cast<int>(r * 0.6f), static_cast<int>(g * 0.6f), static_cast<int>(b * 0.6f)});
        m_paletaTituloAscii.push_back({"░", static_cast<int>(r * 0.4f), static_cast<int>(g * 0.4f), static_cast<int>(b * 0.4f)});
        m_paletaTituloAscii.push_back({"*", r, g, b});
    }
}

void UIDynamicBox::Render(D2DRenderer* d2d, D2D1_COLOR_F corBase, float opacidadeFundo, float espessuraBorda, D2D1_COLOR_F corBorda, float padding, float centerOnX, float centerOnY) {
    if (minX > maxX || minY > maxY) return; // Empty box

    float topPadding = padding;
    float asciiW = 0.0f;
    float asciiH = 0.0f;
    
    if (!m_tituloAscii.empty()) {
        float maxW = 0;
        for (const auto& l : m_tituloAscii) {
            std::string line = l;
            while (!line.empty() && (line.back() == ' ' || line.back() == '\r')) {
                line.pop_back();
            }
            float chars = 0;
            for (size_t lx = 0; lx < line.size(); ) {
                unsigned char c = line[lx];
                int charLen = 1;
                if ((c & 0x80) == 0) charLen = 1;
                else if ((c & 0xE0) == 0xC0) charLen = 2;
                else if ((c & 0xF0) == 0xE0) charLen = 3;
                else if ((c & 0xF8) == 0xF0) charLen = 4;
                lx += charLen;
                chars++;
            }
            if (chars > maxW) maxW = chars;
        }
        asciiW = maxW * m_tituloAsciiScale;
        asciiH = m_tituloAscii.size() * m_tituloAsciiScale * 1.5f;
        
        if (asciiH / 2.0f + 15.0f > padding) {
            topPadding = asciiH / 2.0f + 15.0f;
        }
        
        // Expand horizontal bounds if ascii title is wider than the box
        float currentW = (maxX - minX);
        if (asciiW > currentW) {
            float diff = (asciiW - currentW) / 2.0f;
            minX -= diff;
            maxX += diff;
        }
    }

    float boxX = minX - padding;
    float boxY = minY - topPadding;
    float boxW = (maxX - minX) + padding * 2.0f;
    // Make bottom padding equal to top padding so it looks vertically centered!
    float boxH = (maxY - minY) + topPadding * 2.0f;

    float offsetX = 0.0f;
    if (centerOnX >= 0.0f) {
        float currentCenter = (boxX + boxX + boxW) / 2.0f;
        offsetX = centerOnX - currentCenter;
        boxX += offsetX;
    }

    float offsetY = 0.0f;
    if (centerOnY >= 0.0f) {
        float currentCenterY = (boxY + boxY + boxH) / 2.0f;
        offsetY = centerOnY - currentCenterY;
        boxY += offsetY;
    }

    if (!m_tituloAscii.empty() || !m_titulo.empty()) {
        corBorda = m_corTitulo;
    }

    UIRenderer2D::DrawBox(d2d, boxX, boxY, boxW, boxH, corBase, opacidadeFundo, espessuraBorda, corBorda);

    if (!m_tituloAscii.empty()) {
        float tX = boxX + (boxW - asciiW) / 2.0f;
        float tY = boxY - asciiH / 2.0f;
        
        UIRenderer2D::DrawBox(d2d, tX - 10.0f, tY - 10.0f, asciiW + 20.0f, asciiH + 20.0f, corBase, opacidadeFundo, espessuraBorda, corBorda);
        UIRenderer2D::DrawPixelArt(d2d, m_tituloAscii, m_paletaTituloAscii, tX, tY, m_tituloAsciiScale, 1.0f);
    } else if (!m_titulo.empty()) {
        float titleSize = 22.0f;
        float titleW = m_titulo.length() * titleSize * 0.65f;
        float tX = boxX + (boxW - titleW) / 2.0f;
        float tY = boxY - titleSize * 0.6f;
        
        UIRenderer2D::DrawBox(d2d, tX - 15.0f, tY - 10.0f, titleW + 30.0f, titleSize + 20.0f, corBase, opacidadeFundo, espessuraBorda, corBorda);
        UIRenderer2D::DrawTextNative(d2d, m_titulo, boxX + boxW/2.0f, tY, m_corTitulo, titleSize, true);
    }

    for (const auto& r : rects) {
        d2d->preencherRetangulo(r.x + offsetX, r.y + offsetY, r.w, r.h, r.cor);
    }

    for (const auto& a : arts) {
        UIRenderer2D::DrawPixelArt(d2d, a.arte, a.paleta, a.x + offsetX, a.y + offsetY, a.scale, a.opacity);
    }
    
    for (const auto& t : texts) {
        if (t.visible) {
            UIRenderer2D::DrawTextNative(d2d, t.text, t.x + offsetX, t.y + offsetY, t.color, t.size, t.center);
        }
    }
}

void UIDynamicBox::Clear() {
    texts.clear();
    arts.clear();
    rects.clear();
    texts.reserve(16);
    arts.reserve(8);
    rects.reserve(8);
    minX = 999999.0f;
    minY = 999999.0f;
    maxX = -999999.0f;
    maxY = -999999.0f;
    m_titulo.clear();
    m_tituloAscii.clear();
    m_paletaTituloAscii.clear();
}
