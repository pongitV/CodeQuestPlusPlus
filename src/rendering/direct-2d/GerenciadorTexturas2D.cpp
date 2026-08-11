#include "GerenciadorTexturas2D.h"
#include <iostream>
#include <vector>

std::unordered_map<std::string, ID2D1Bitmap*> GerenciadorTexturas2D::s_cacheTexturas;
std::unordered_set<std::string> GerenciadorTexturas2D::s_caminhosInvalidos;
IWICImagingFactory* GerenciadorTexturas2D::s_wicFactory = nullptr;

std::wstring GerenciadorTexturas2D::resolverCaminhoWIC(const std::string& caminhoUtf8) {
    std::wstring caminhoW(caminhoUtf8.begin(), caminhoUtf8.end());

    // Se existe diretamente no caminho relativo
    if (GetFileAttributesW(caminhoW.c_str()) != INVALID_FILE_ATTRIBUTES) {
        return caminhoW;
    }

    // Tenta ../caminho
    std::wstring relParent = L"../" + caminhoW;
    if (GetFileAttributesW(relParent.c_str()) != INVALID_FILE_ATTRIBUTES) {
        return relParent;
    }

    // Tenta caminho absoluto baseado na pasta do .exe
    wchar_t exePath[MAX_PATH];
    if (GetModuleFileNameW(nullptr, exePath, MAX_PATH)) {
        wchar_t* lastSlash = wcsrchr(exePath, L'\\');
        if (!lastSlash) lastSlash = wcsrchr(exePath, L'/');
        if (lastSlash) {
            *lastSlash = L'\0';
            std::wstring absPath = std::wstring(exePath) + L"/" + caminhoW;
            if (GetFileAttributesW(absPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
                return absPath;
            }
            wchar_t* parentSlash = wcsrchr(exePath, L'\\');
            if (!parentSlash) parentSlash = wcsrchr(exePath, L'/');
            if (parentSlash) {
                *parentSlash = L'\0';
                std::wstring parentAbsPath = std::wstring(exePath) + L"/" + caminhoW;
                if (GetFileAttributesW(parentAbsPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
                    return parentAbsPath;
                }
            }
        }
    }

    return caminhoW;
}

ID2D1Bitmap* GerenciadorTexturas2D::obterTextura(ID2D1RenderTarget* rt, const std::string& caminhoAsset) {
    if (!rt || caminhoAsset.empty()) return nullptr;

    auto it = s_cacheTexturas.find(caminhoAsset);
    if (it != s_cacheTexturas.end()) {
        return it->second;
    }

    if (s_caminhosInvalidos.find(caminhoAsset) != s_caminhosInvalidos.end()) {
        return nullptr;
    }

    if (!s_wicFactory) {
        CoInitialize(nullptr);
        HRESULT hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&s_wicFactory)
        );
        if (FAILED(hr) || !s_wicFactory) {
            return nullptr;
        }
    }

    std::wstring caminhoFinal = resolverCaminhoWIC(caminhoAsset);

    IWICBitmapDecoder* decoder = nullptr;
    HRESULT hr = s_wicFactory->CreateDecoderFromFilename(
        caminhoFinal.c_str(),
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnDemand,
        &decoder
    );
    if (FAILED(hr) || !decoder) {
        s_caminhosInvalidos.insert(caminhoAsset);
        return nullptr;
    }

    IWICBitmapFrameDecode* frame = nullptr;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr) || !frame) {
        decoder->Release();
        s_caminhosInvalidos.insert(caminhoAsset);
        return nullptr;
    }

    IWICFormatConverter* converter = nullptr;
    hr = s_wicFactory->CreateFormatConverter(&converter);
    if (FAILED(hr) || !converter) {
        frame->Release();
        decoder->Release();
        s_caminhosInvalidos.insert(caminhoAsset);
        return nullptr;
    }

    hr = converter->Initialize(
        frame,
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0,
        WICBitmapPaletteTypeCustom
    );

    ID2D1Bitmap* bitmap = nullptr;
    if (SUCCEEDED(hr)) {
        rt->CreateBitmapFromWicBitmap(converter, nullptr, &bitmap);
    }

    converter->Release();
    frame->Release();
    decoder->Release();

    if (bitmap) {
        s_cacheTexturas[caminhoAsset] = bitmap;
    } else {
        s_caminhosInvalidos.insert(caminhoAsset);
    }

    return bitmap;
}

void GerenciadorTexturas2D::desenharImagem(
    ID2D1RenderTarget* rt, 
    const std::string& caminhoAsset, 
    float x, float y, 
    float largura, float altura, 
    float opacidade
) {
    ID2D1Bitmap* bitmap = obterTextura(rt, caminhoAsset);
    if (!bitmap || !rt) return;

    D2D1_RECT_F destRect = D2D1::RectF(x, y, x + largura, y + altura);
    rt->DrawBitmap(
        bitmap,
        destRect,
        opacidade,
        D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
    );
}

void GerenciadorTexturas2D::desenharImagemComEfeito(
    ID2D1RenderTarget* rt,
    const std::string& caminhoAsset,
    float x, float y,
    float largura, float altura,
    D2D1_COLOR_F tintCor,
    float opacidade
) {
    ID2D1Bitmap* bitmap = obterTextura(rt, caminhoAsset);
    if (!bitmap || !rt) return;

    D2D1_RECT_F destRect = D2D1::RectF(x, y, x + largura, y + altura);
    rt->DrawBitmap(
        bitmap,
        destRect,
        opacidade,
        D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
    );

    // Desenha overlay colorido para efeito de status (ex: congelado, sangramento, veneno)
    ID2D1SolidColorBrush* brush = nullptr;
    if (SUCCEEDED(rt->CreateSolidColorBrush(tintCor, &brush))) {
        rt->FillRectangle(destRect, brush);
        brush->Release();
    }
}

void GerenciadorTexturas2D::limpar() {
    for (auto& pair : s_cacheTexturas) {
        if (pair.second) {
            pair.second->Release();
        }
    }
    s_cacheTexturas.clear();
    s_caminhosInvalidos.clear();

    if (s_wicFactory) {
        s_wicFactory->Release();
        s_wicFactory = nullptr;
    }
}
