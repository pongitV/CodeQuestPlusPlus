#include "D2DRenderer.h"
#include <algorithm>
#include <cstring>


D2DRenderer::D2DRenderer()
    : m_backbuffer(BACKBUFFER_WIDTH * BACKBUFFER_HEIGHT, 0xFF000000)
{
}

D2DRenderer::~D2DRenderer() {
    for (auto& pair : m_fontCache) {
        if (pair.second) pair.second->Release();
    }
    m_fontCache.clear();

    for (auto& pair : m_colorBrushCache) {
        if (pair.second) pair.second->Release();
    }
    m_colorBrushCache.clear();

    if (m_texturaBackbuffer) m_texturaBackbuffer->Release();
    if (m_fontePadrao) m_fontePadrao->Release();
    if (m_dwriteFactory) m_dwriteFactory->Release();
    if (m_brush) m_brush->Release();
    if (m_renderTarget) m_renderTarget->Release();
    if (m_factory) m_factory->Release();
}

bool D2DRenderer::initialize(HWND hwnd) {
    m_hwnd = hwnd;

    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_factory);
    if (FAILED(hr)) return false;

    RECT rc;
    GetClientRect(hwnd, &rc);

    hr = m_factory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)),
        D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
        &m_renderTarget
    );
    if (FAILED(hr)) return false;

    hr = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1), &m_brush);
    if (FAILED(hr)) return false;

    m_renderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);

    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&m_dwriteFactory));
    if (FAILED(hr)) return false;

    hr = m_dwriteFactory->CreateTextFormat(
        L"Consolas", nullptr,
        DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        14.0f, L"en-US", &m_fontePadrao
    );
    if (FAILED(hr)) return false;

    m_fontePadrao->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    m_fontePadrao->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

    D2D1_BITMAP_PROPERTIES bmpProps = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
        96.0f, 96.0f
    );
    hr = m_renderTarget->CreateBitmap(
        D2D1::SizeU(BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT),
        m_backbuffer.data(), BACKBUFFER_WIDTH * 4,
        &bmpProps, &m_texturaBackbuffer
    );
    if (FAILED(hr)) return false;

    return true;
}

void D2DRenderer::redimensionar(HWND hwnd) {
    if (m_renderTarget) {
        RECT rc;
        GetClientRect(hwnd, &rc);
        m_renderTarget->Resize(D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top));
    }
}

void D2DRenderer::comecarQuadro() {
    m_renderTarget->BeginDraw();
}

void D2DRenderer::finalizarQuadro() {
    m_renderTarget->EndDraw();
}

void D2DRenderer::limpar(D2D1_COLOR_F cor) {
    m_renderTarget->Clear(cor);
}

ID2D1SolidColorBrush* D2DRenderer::obterSolidBrushCache(D2D1_COLOR_F cor) {
    if (!m_renderTarget) return m_brush;

    uint32_t r = static_cast<uint8_t>(std::max(0.0f, std::min(1.0f, cor.r)) * 255.0f);
    uint32_t g = static_cast<uint8_t>(std::max(0.0f, std::min(1.0f, cor.g)) * 255.0f);
    uint32_t b = static_cast<uint8_t>(std::max(0.0f, std::min(1.0f, cor.b)) * 255.0f);
    uint32_t a = static_cast<uint8_t>(std::max(0.0f, std::min(1.0f, cor.a)) * 255.0f);
    uint32_t key = (a << 24) | (r << 16) | (g << 8) | b;

    auto it = m_colorBrushCache.find(key);
    if (it != m_colorBrushCache.end()) {
        return it->second;
    }

    ID2D1SolidColorBrush* brush = nullptr;
    HRESULT hr = m_renderTarget->CreateSolidColorBrush(cor, &brush);
    if (SUCCEEDED(hr) && brush) {
        m_colorBrushCache[key] = brush;
        return brush;
    }
    if (m_brush) m_brush->SetColor(cor);
    return m_brush;
}

void D2DRenderer::preencherRetangulo(float x, float y, float w, float h, D2D1_COLOR_F cor) {
    if (!m_renderTarget) return;
    ID2D1SolidColorBrush* brush = obterSolidBrushCache(cor);
    if (brush) {
        m_renderTarget->FillRectangle(D2D1::RectF(x, y, x + w, y + h), brush);
    }
}

void D2DRenderer::preencherRetanguloGradiente(float x, float y, float w, float h, D2D1_COLOR_F corTopo, D2D1_COLOR_F corBase) {
    if (!m_renderTarget) return;
    ID2D1GradientStopCollection* pGradientStops = nullptr;
    D2D1_GRADIENT_STOP gradientStops[2];
    gradientStops[0].color = corTopo;
    gradientStops[0].position = 0.0f;
    gradientStops[1].color = corBase;
    gradientStops[1].position = 1.0f;
    HRESULT hr = m_renderTarget->CreateGradientStopCollection(
        gradientStops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pGradientStops);
    if (SUCCEEDED(hr)) {
        ID2D1LinearGradientBrush* pLinBrush = nullptr;
        hr = m_renderTarget->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(D2D1::Point2F(x, y), D2D1::Point2F(x, y + h)),
            pGradientStops, &pLinBrush);
        if (SUCCEEDED(hr)) {
            m_renderTarget->FillRectangle(D2D1::RectF(x, y, x + w, y + h), pLinBrush);
            pLinBrush->Release();
        }
        pGradientStops->Release();
    }
}

void D2DRenderer::desenharRetangulo(float x, float y, float w, float h, D2D1_COLOR_F cor, float espessura) {
    if (!m_renderTarget) return;
    ID2D1SolidColorBrush* brush = obterSolidBrushCache(cor);
    if (brush) {
        m_renderTarget->DrawRectangle(D2D1::RectF(x, y, x + w, y + h), brush, espessura);
    }
}

void D2DRenderer::drawRectangle(
    float x, float y, float w, float h,
    D2D1_COLOR_F cor,
    float espessura,
    bool preencher,
    bool isGradient,
    D2D1_COLOR_F corBase
) {
    if (isGradient) {
        preencherRetanguloGradiente(x, y, w, h, cor, corBase);
        if (!preencher) {
            desenharRetangulo(x, y, w, h, cor, espessura);
        }
    } else if (preencher) {
        preencherRetangulo(x, y, w, h, cor);
    } else {
        desenharRetangulo(x, y, w, h, cor, espessura);
    }
}

void D2DRenderer::desenharLinha(float x1, float y1, float x2, float y2, D2D1_COLOR_F cor, float espessura) {
    if (!m_renderTarget || !m_brush) return;
    m_brush->SetColor(cor);
    m_renderTarget->DrawLine(D2D1::Point2F(x1, y1), D2D1::Point2F(x2, y2), m_brush, espessura);
}

void D2DRenderer::preencherElipse(float x, float y, float radiusX, float radiusY, D2D1_COLOR_F cor) {
    if (!m_renderTarget || !m_brush) return;
    m_brush->SetColor(cor);
    D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radiusX, radiusY);
    m_renderTarget->FillEllipse(ellipse, m_brush);
}

IDWriteTextFormat* D2DRenderer::obterFormatoTextoCache(
    const std::wstring& fontName,
    float size,
    DWRITE_FONT_WEIGHT weight,
    DWRITE_FONT_STYLE style,
    DWRITE_TEXT_ALIGNMENT align,
    DWRITE_PARAGRAPH_ALIGNMENT pAlign
) {
    if (!m_dwriteFactory) return nullptr;

    FontKey key{ fontName, size, weight, style, align, pAlign };
    auto it = m_fontCache.find(key);
    if (it != m_fontCache.end()) {
        return it->second;
    }

    IDWriteTextFormat* fmt = nullptr;
    HRESULT hr = m_dwriteFactory->CreateTextFormat(
        fontName.c_str(),
        nullptr,
        weight,
        style,
        DWRITE_FONT_STRETCH_NORMAL,
        size,
        L"en-US",
        &fmt
    );

    if (SUCCEEDED(hr) && fmt) {
        fmt->SetTextAlignment(align);
        fmt->SetParagraphAlignment(pAlign);
        fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        m_fontCache[key] = fmt;
        return fmt;
    }
    return nullptr;
}

void D2DRenderer::desenharTexto(const std::wstring& texto, float x, float y, D2D1_COLOR_F cor, float tamanhoFonte, IDWriteTextFormat* formato) {
    if (texto.empty() || !m_renderTarget || !m_brush) return;
    m_brush->SetColor(cor);

    IDWriteTextFormat* fmt = formato;
    if (!fmt) {
        float size = (tamanhoFonte > 0.0f) ? tamanhoFonte : 14.0f;
        fmt = obterFormatoTextoCache(L"Consolas", size, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    }

    if (!fmt) fmt = m_fontePadrao;

    const float LOGICAL_W = 1920.0f;
    const float LOGICAL_H = 1080.0f;
    float right  = std::max(x + 1.0f, LOGICAL_W);
    float bottom = std::max(y + 1.0f, LOGICAL_H);

    m_renderTarget->DrawText(texto.c_str(), (UINT32)texto.size(), fmt,
        D2D1::RectF(x, y, right, bottom), m_brush);
}

void D2DRenderer::desenharTextoCentralizado(const std::wstring& texto, float x, float y, float w, float h, D2D1_COLOR_F cor, float tamanhoFonte) {
    if (texto.empty() || !m_renderTarget || !m_brush) return;
    m_brush->SetColor(cor);
    
    float size = (tamanhoFonte > 0.0f) ? tamanhoFonte : 14.0f;
    IDWriteTextFormat* fmt = obterFormatoTextoCache(L"Consolas", size, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    if (!fmt) fmt = m_fontePadrao;

    m_renderTarget->DrawText(texto.c_str(), (UINT32)texto.size(), fmt,
        D2D1::RectF(x, y, x + w, y + h), m_brush);
}

void D2DRenderer::gravarPixelBackbuffer(int x, int y, uint32_t argb) {
    if (x >= 0 && x < BACKBUFFER_WIDTH && y >= 0 && y < BACKBUFFER_HEIGHT) {
        m_backbuffer[y * BACKBUFFER_WIDTH + x] = argb;
    }
}

void D2DRenderer::copiarBackbufferParaTextura() {
    if (m_texturaBackbuffer && m_renderTarget) {
        D2D1_RECT_U rect = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
        m_texturaBackbuffer->CopyFromMemory(&rect, m_backbuffer.data(), BACKBUFFER_WIDTH * 4);
    }
}

void D2DRenderer::apresentarBackbuffer(D2D1_COLOR_F corLimpeza) {
    if (!m_renderTarget) return;
    m_renderTarget->BeginDraw();
    m_renderTarget->Clear(corLimpeza);
    if (m_texturaBackbuffer) {
        D2D1_SIZE_U tsz = m_texturaBackbuffer->GetPixelSize();
        D2D1_SIZE_F rsz = m_renderTarget->GetSize();
        m_renderTarget->DrawBitmap(m_texturaBackbuffer,
            D2D1::RectF(0, 0, rsz.width, rsz.height), 1.0f,
            D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
            D2D1::RectF(0, 0, (FLOAT)tsz.width, (FLOAT)tsz.height));
    }
    m_renderTarget->EndDraw();
}
