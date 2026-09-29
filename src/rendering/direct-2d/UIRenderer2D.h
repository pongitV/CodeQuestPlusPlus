#pragma once

#include "D2DRenderer.h"
#include <string>
#include <vector>

struct GrupoCorUI {
    std::string chars;
    int r, g, b;
};

class UIRenderer2D {
public:
    static void DrawBox(D2DRenderer* d2d, float x, float y, float w, float h, D2D1_COLOR_F corBase, float opacidade = 1.0f, float espessuraBorda = 0.0f, D2D1_COLOR_F corBorda = D2D1::ColorF(0,0,0));
    
    // Escala define o tamanho de cada pixel da arte em pixels.
    static void DrawPixelArt(D2DRenderer* d2d, const std::vector<std::string>& arte, const std::vector<GrupoCorUI>& paleta, float x, float y, float pixelScale, float opacidade = 1.0f, float pixelScaleY = -1.0f);
    static void DrawPixelArtWave(D2DRenderer* d2d, const std::vector<std::string>& arte, const std::vector<GrupoCorUI>& paleta, float x, float y, float pixelScale, float time, float amplitude = 10.0f, float frequency = 0.1f, float opacidade = 1.0f, float pixelScaleY = -1.0f);
    
    // x e y no espaco logico 1920x1080
    static void DrawTextNative(D2DRenderer* d2d, const std::wstring& texto, float x, float y, D2D1_COLOR_F cor, float tamanhoFonte, bool centralizado = false, float maxWidth = 1920.0f);
    
    static void SetupTransform(D2DRenderer* d2d, float windowWidth, float windowHeight);
    static void ResetTransform(D2DRenderer* d2d);

    static constexpr float LOGICAL_WIDTH = 1920.0f;
    static constexpr float LOGICAL_HEIGHT = 1080.0f;
};

class UIDynamicBox {
private:
    struct TextElement {
        std::wstring text;
        float x, y, size;
        D2D1_COLOR_F color;
        bool center;
        bool visible;
    };
    struct ArtElement {
        std::vector<std::string> arte;
        std::vector<GrupoCorUI> paleta;
        float x, y, scale, opacity;
    };
    struct RectElement {
        float x, y, w, h;
        D2D1_COLOR_F cor;
    };
    
    std::vector<TextElement> texts;
    std::vector<ArtElement> arts;
    std::vector<RectElement> rects;

    float minX = 999999.0f;
    float minY = 999999.0f;
    float maxX = -999999.0f;
    float maxY = -999999.0f;

    std::wstring m_titulo;
    D2D1_COLOR_F m_corTitulo = D2D1::ColorF(1.0f, 1.0f, 1.0f);
    std::vector<std::string> m_tituloAscii;
    std::vector<GrupoCorUI> m_paletaTituloAscii;
    float m_tituloAsciiScale = 4.0f;

    void updateBounds(float x1, float y1, float x2, float y2);

public:
    void AddText(const std::wstring& text, float x, float y, float size, D2D1_COLOR_F color, bool center = false, bool visible = true, float customWidth = 0.0f);
    void AddPixelArt(const std::vector<std::string>& arte, const std::vector<GrupoCorUI>& paleta, float x, float y, float scale, float opacity = 1.0f);
    void AddRect(float x, float y, float w, float h, D2D1_COLOR_F cor);
    void SetTitle(const std::wstring& titulo, D2D1_COLOR_F cor = D2D1::ColorF(1.0f, 1.0f, 1.0f));
    void SetTitleAscii(const std::vector<std::string>& tituloAscii, const std::vector<GrupoCorUI>& paleta, float scale = 4.0f);
    void Clear();
    void Render(D2DRenderer* d2d, D2D1_COLOR_F corBase = D2D1::ColorF(0.0f, 0.0f, 0.0f), float opacidadeFundo = 0.85f, float espessuraBorda = 2.0f, D2D1_COLOR_F corBorda = D2D1::ColorF(1.0f, 1.0f, 1.0f), float padding = 20.0f, float centerOnX = -1.0f, float centerOnY = -1.0f);
};
