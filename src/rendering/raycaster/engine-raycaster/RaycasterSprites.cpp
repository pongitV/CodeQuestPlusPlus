#include "RaycasterSprites.h"
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
static void parseAnsiColors(const std::string& ansi, SpritePixel& pixel) {
    (void)ansi;
    (void)pixel;
}

SpriteCache RaycasterSprites::parseArte(const std::vector<std::string>& raw) {
    SpriteCache sc;
    sc.height = raw.size();
    sc.width = 0;
    std::vector<std::vector<SpritePixel>> tempPixels;
    tempPixels.resize(sc.height);
    for(int i=0; i<sc.height; ++i) {
        std::string currentAnsi = "";
        std::string line = raw[i];
        for(size_t j=0; j<line.length(); ) {
            int len = 1;
            unsigned char c = line[j];
            if ((c & 0x80) == 0) len = 1;
            else if ((c & 0xE0) == 0xC0) len = 2;
            else if ((c & 0xF0) == 0xE0) len = 3;
            else if ((c & 0xF8) == 0xF0) len = 4;
            
            std::string pixelChar = line.substr(j, len);
            SpritePixel sp;
            if (pixelChar == " " || pixelChar == "") {
                sp.isTransparente = true;
            } else {
                sp.isTransparente = false;
                sp.ch = ' ';
            }
            tempPixels[i].push_back(sp);
            j += len;
        }
        if((int)tempPixels[i].size() > sc.width) sc.width = tempPixels[i].size();
    }
    
    sc.pixels.resize(sc.width * sc.height);
    for (int y = 0; y < sc.height; ++y) {
        for (int x = 0; x < sc.width; ++x) {
            if (x < (int)tempPixels[y].size()) {
                sc.pixels[y * sc.width + x] = tempPixels[y][x];
            } else {
                SpritePixel blank;
                blank.isTransparente = true;
                sc.pixels[y * sc.width + x] = blank;
            }
        }
    }
    
    return sc;
}

SpriteCache RaycasterSprites::parseSprite(const std::vector<std::string>& raw, int r, int g, int b, bool isMahoraga) {
    SpriteCache sc;
    sc.height = raw.size();
    sc.width = 0;
    
    int minX = 999999, maxX = -1;
    
    for (int i = 0; i < sc.height; ++i) {
        std::string line = raw[i];
        int j_char = 0;
        for (size_t j = 0; j < line.length(); ) {
            int len = 1;
            unsigned char c = line[j];
            if ((c & 0x80) == 0) len = 1;
            else if ((c & 0xE0) == 0xC0) len = 2;
            else if ((c & 0xF0) == 0xE0) len = 3;
            else if ((c & 0xF8) == 0xF0) len = 4;
            
            std::string pixelChar = line.substr(j, len);
            if (pixelChar != " " && pixelChar != "") {
                if (j_char < minX) minX = j_char;
                if (j_char > maxX) maxX = j_char;
            }
            j_char++;
            j += len;
        }
        if (j_char > sc.width) sc.width = j_char;
    }
    
    if (minX > maxX) { minX = 0; maxX = sc.width - 1; }
    
    sc.width = maxX - minX + 1;
    std::vector<std::vector<SpritePixel>> tempPixels;
    tempPixels.resize(sc.height);
    
    for (int i = 0; i < sc.height; ++i) {
        std::string line = raw[i];
        int j_char = 0;
        for (size_t j = 0; j < line.length(); ) {
            int len = 1;
            unsigned char c_utf = line[j];
            if ((c_utf & 0x80) == 0) len = 1;
            else if ((c_utf & 0xE0) == 0xC0) len = 2;
            else if ((c_utf & 0xF0) == 0xE0) len = 3;
            else if ((c_utf & 0xF8) == 0xF0) len = 4;
            
            std::string pixelChar = line.substr(j, len);
            
            if (j_char >= minX && j_char <= maxX) {
                SpritePixel sp;
                if (pixelChar == " " || pixelChar == "") {
                    sp.isTransparente = true;
                } else {
                    char c = pixelChar[0];
                    int currentBaseR = r;
                    int currentBaseG = g;
                    int currentBaseB = b;
                    if (isMahoraga && i < 24) {
                        currentBaseR = 255;
                        currentBaseG = 215;
                        currentBaseB = 0;
                    }
                    int rMod = currentBaseR, gMod = currentBaseG, bMod = currentBaseB;
                    if (c == '@' || c == 'M' || c == 'W' || c == '#' || c == '&' || c == '8') { rMod = currentBaseR * 0.4; gMod = currentBaseG * 0.4; bMod = currentBaseB * 0.4; }
                    else if (c == '%' || c == 'O' || c == 'X' || c == 'S' || c == 'Q') { rMod = currentBaseR * 0.6; gMod = currentBaseG * 0.6; bMod = currentBaseB * 0.6; }
                    else if (c == '*' || c == '+' || c == 'x' || c == 'o' || c == '=' || c == 'H') { rMod = currentBaseR * 0.8; gMod = currentBaseG * 0.8; bMod = currentBaseB * 0.8; }
                    else if (c == '-' || c == '~' || c == ':' || c == ';') { rMod = std::min(255, (int)(currentBaseR * 1.2)); gMod = std::min(255, (int)(currentBaseG * 1.2)); bMod = std::min(255, (int)(currentBaseB * 1.2)); }
                    else if (c == '.' || c == ',' || c == '\'') { rMod = std::min(255, (int)(currentBaseR * 1.5)); gMod = std::min(255, (int)(currentBaseG * 1.5)); bMod = std::min(255, (int)(currentBaseB * 1.5)); }
                    else if (c == '_' || c == '|' || c == '\\' || c == '/' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == '<' || c == '>') { rMod = currentBaseR * 0.5; gMod = currentBaseG * 0.5; bMod = currentBaseB * 0.5; }
                    
                    sp.isTransparente = false;
                    sp.r = rMod;
                    sp.g = gMod;
                    sp.b = bMod;
                    sp.ch = ' ';
                    sp.hasBg = true;
                }
                tempPixels[i].push_back(sp);
            }
            j_char++;
            j += len;
        }
        
        while (j_char <= maxX) {
            if (j_char >= minX) {
                SpritePixel sp;
                sp.isTransparente = true;
                tempPixels[i].push_back(sp);
            }
            j_char++;
        }
    }
    
    // Compressão de resolução (downsampling) para melhorar o desempenho
    std::vector<std::vector<SpritePixel>> compressedTempPixels;
    int scale = 1; // Fator de compressão global para manter paridade com o terminal
    int compWidth = std::max(1, sc.width / scale);
    int compHeight = std::max(1, sc.height / scale);
    compressedTempPixels.resize(compHeight);
    
    for (int y = 0; y < compHeight; ++y) {
        for (int x = 0; x < compWidth; ++x) {
            SpritePixel bestPixel;
            bestPixel.isTransparente = true;
            for(int dy = 0; dy < scale; ++dy) {
                for(int dx = 0; dx < scale; ++dx) {
                    int oy = y * scale + dy;
                    int ox = x * scale + dx;
                    if (oy < sc.height && ox < sc.width) {
                        if (!tempPixels[oy][ox].isTransparente) {
                            bestPixel = tempPixels[oy][ox];
                            goto foundSprite;
                        }
                    }
                }
            }
            foundSprite:
            compressedTempPixels[y].push_back(bestPixel);
        }
    }
    
    SpriteCache finalSc;
    finalSc.width = compWidth;
    finalSc.height = compHeight;
    finalSc.pixels.resize(compWidth * compHeight);
    
    for (int y = 0; y < compHeight; ++y) {
        for (int x = 0; x < compWidth; ++x) {
            if (compressedTempPixels[y][x].isTransparente) {
                SpritePixel sp;
                sp.isTransparente = true;
                finalSc.pixels[y * compWidth + x] = sp;
            } else {
                bool isEdge = false;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) continue;
                        int ny = y + dy;
                        int nx = x + dx;
                        if (ny < 0 || ny >= compHeight || nx < 0 || nx >= compWidth) isEdge = true;
                        else if (compressedTempPixels[ny][nx].isTransparente) isEdge = true;
                    }
                }
                if (isEdge) {
                    SpritePixel sp;
                    sp.isTransparente = false;
                    sp.r = 0; sp.g = 0; sp.b = 0;
                    sp.ch = ' ';
                    sp.hasBg = true;
                    finalSc.pixels[y * compWidth + x] = sp;
                } else {
                    finalSc.pixels[y * compWidth + x] = compressedTempPixels[y][x];
                }
            }
        }
    }
    
    return finalSc;
}

std::vector<std::string> RaycasterSprites::colorirArte(const std::vector<std::string>& arte, const std::string& corAnsi) {
    (void)corAnsi;
    return arte;
}

#include <windows.h>
#include <wincodec.h>

SpriteCache RaycasterSprites::carregarSpritePNG(const std::string& caminhoAsset) {
    SpriteCache sc;
    IWICImagingFactory* wicFactory = nullptr;
    CoInitialize(nullptr);
    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&wicFactory)
    );
    if (FAILED(hr) || !wicFactory) return sc;

    std::wstring caminhoW(caminhoAsset.begin(), caminhoAsset.end());
    IWICBitmapDecoder* decoder = nullptr;
    hr = wicFactory->CreateDecoderFromFilename(
        caminhoW.c_str(),
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnDemand,
        &decoder
    );
    if (FAILED(hr) || !decoder) {
        std::wstring relParent = L"../" + caminhoW;
        hr = wicFactory->CreateDecoderFromFilename(
            relParent.c_str(),
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnDemand,
            &decoder
        );
        if (FAILED(hr) || !decoder) {
            wicFactory->Release();
            return sc;
        }
    }

    IWICBitmapFrameDecode* frame = nullptr;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr) || !frame) {
        decoder->Release();
        wicFactory->Release();
        return sc;
    }

    IWICFormatConverter* converter = nullptr;
    hr = wicFactory->CreateFormatConverter(&converter);
    if (FAILED(hr) || !converter) {
        frame->Release();
        decoder->Release();
        wicFactory->Release();
        return sc;
    }

    hr = converter->Initialize(
        frame,
        GUID_WICPixelFormat32bppBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0,
        WICBitmapPaletteTypeCustom
    );

    if (SUCCEEDED(hr)) {
        UINT width = 0, height = 0;
        converter->GetSize(&width, &height);
        sc.width = (int)width;
        sc.height = (int)height;
        sc.pixels.resize(width * height);

        std::vector<BYTE> buffer(width * height * 4);
        converter->CopyPixels(nullptr, width * 4, buffer.size(), buffer.data());

        for (UINT y = 0; y < height; ++y) {
            for (UINT x = 0; x < width; ++x) {
                size_t idx = (y * width + x) * 4;
                BYTE b = buffer[idx + 0];
                BYTE g = buffer[idx + 1];
                BYTE r = buffer[idx + 2];
                BYTE a = buffer[idx + 3];

                SpritePixel sp;
                if (a < 30) {
                    sp.isTransparente = true;
                } else {
                    sp.isTransparente = false;
                    sp.r = r;
                    sp.g = g;
                    sp.b = b;
                    sp.ch = ' ';
                    sp.hasBg = true;
                }
                sc.pixels[y * width + x] = sp;
            }
        }
    }

    converter->Release();
    frame->Release();
    decoder->Release();
    wicFactory->Release();
    return sc;
}
