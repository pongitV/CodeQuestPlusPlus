#pragma once

#include <cstdint>
#include <stdint.h>
#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "../config/RenderingConfig.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

class D2DRenderer {
public:
    static constexpr int BACKBUFFER_WIDTH = RenderingConfig::BACKBUFFER_WIDTH;
    static constexpr int BACKBUFFER_HEIGHT = RenderingConfig::BACKBUFFER_HEIGHT;

    D2DRenderer();
    ~D2DRenderer();

    bool initialize(HWND hwnd);
    void redimensionar(HWND hwnd);

    void comecarQuadro();
    void finalizarQuadro();

    void limpar(D2D1_COLOR_F cor = D2D1::ColorF(0, 0, 0));

    void preencherRetangulo(float x, float y, float w, float h, D2D1_COLOR_F cor);
    void preencherRetanguloGradiente(float x, float y, float w, float h, D2D1_COLOR_F corTopo, D2D1_COLOR_F corBase);
    void desenharRetangulo(float x, float y, float w, float h, D2D1_COLOR_F cor, float espessura = 1.0f);
    void drawRectangle(
        float x, float y, float w, float h,
        D2D1_COLOR_F cor,
        float espessura = 1.0f,
        bool preencher = true,
        bool isGradient = false,
        D2D1_COLOR_F corBase = D2D1::ColorF(0, 0, 0)
    );
    void desenharLinha(float x1, float y1, float x2, float y2, D2D1_COLOR_F cor, float espessura = 1.0f);
    void preencherElipse(float x, float y, float radiusX, float radiusY, D2D1_COLOR_F cor);
    void desenharTexto(const std::wstring& texto, float x, float y, D2D1_COLOR_F cor, float tamanhoFonte = 16.0f, IDWriteTextFormat* formato = nullptr);
    void desenharTextoCentralizado(const std::wstring& texto, float x, float y, float w, float h, D2D1_COLOR_F cor, float tamanhoFonte = 16.0f);

    void gravarPixelBackbuffer(int x, int y, uint32_t argb);
    void copiarBackbufferParaTextura();
    void apresentarBackbuffer(D2D1_COLOR_F corLimpeza = D2D1::ColorF(0, 0, 0));

    ID2D1Bitmap* obterTexturaBackbuffer() const { return m_texturaBackbuffer; }
    ID2D1RenderTarget* obterRenderTarget() const { return m_renderTarget; }
    IDWriteFactory* obterDWriteFactory() const { return m_dwriteFactory; }
    IDWriteTextFormat* obterFontePadrao() const { return m_fontePadrao; }

    uint32_t* obterBackbuffer() { return m_backbuffer.data(); }

    // English Aliases
    void resize(HWND hwnd) { redimensionar(hwnd); }
    void beginFrame() { comecarQuadro(); }
    void endFrame() { finalizarQuadro(); }
    void clear(D2D1_COLOR_F color = D2D1::ColorF(0, 0, 0)) { limpar(color); }
    ID2D1Bitmap* getBackbufferTexture() const { return obterTexturaBackbuffer(); }
    ID2D1RenderTarget* getRenderTarget() const { return obterRenderTarget(); }
    IDWriteFactory* getDWriteFactory() const { return obterDWriteFactory(); }
    IDWriteTextFormat* getDefaultFont() const { return obterFontePadrao(); }

private:
    ID2D1Factory* m_factory = nullptr;
    ID2D1HwndRenderTarget* m_renderTarget = nullptr;
    ID2D1SolidColorBrush* m_brush = nullptr;
    IDWriteFactory* m_dwriteFactory = nullptr;
    IDWriteTextFormat* m_fontePadrao = nullptr;

    ID2D1Bitmap* m_texturaBackbuffer = nullptr;
    std::vector<uint32_t> m_backbuffer;

    HWND m_hwnd = nullptr;

    struct FontKey {
        std::wstring fontName;
        float size;
        DWRITE_FONT_WEIGHT weight;
        DWRITE_FONT_STYLE style;
        DWRITE_TEXT_ALIGNMENT align;
        DWRITE_PARAGRAPH_ALIGNMENT pAlign;

        bool operator==(const FontKey& o) const {
            return fontName == o.fontName &&
                   size == o.size &&
                   weight == o.weight &&
                   style == o.style &&
                   align == o.align &&
                   pAlign == o.pAlign;
        }
    };

    struct FontKeyHash {
        size_t operator()(const FontKey& k) const {
            size_t h1 = std::hash<std::wstring>{}(k.fontName);
            size_t h2 = std::hash<float>{}(k.size);
            size_t h3 = std::hash<int>{}(static_cast<int>(k.weight));
            size_t h4 = std::hash<int>{}(static_cast<int>(k.align));
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
        }
    };

    std::unordered_map<FontKey, IDWriteTextFormat*, FontKeyHash> m_fontCache;
    IDWriteTextFormat* obterFormatoTextoCache(
        const std::wstring& fontName,
        float size,
        DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL,
        DWRITE_TEXT_ALIGNMENT align = DWRITE_TEXT_ALIGNMENT_LEADING,
        DWRITE_PARAGRAPH_ALIGNMENT pAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR
    );

    std::unordered_map<uint32_t, ID2D1SolidColorBrush*> m_colorBrushCache;
    ID2D1SolidColorBrush* obterSolidBrushCache(D2D1_COLOR_F cor);
};
