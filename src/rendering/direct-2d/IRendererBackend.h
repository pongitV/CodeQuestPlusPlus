#pragma once

#include <cstdint>
#include <string>

struct CorRGBA {
    uint8_t r = 0, g = 0, b = 0, a = 255;
};

class IRendererBackend {
public:
    virtual ~IRendererBackend() = default;

    virtual void limparTela() = 0;
    virtual void gravarPixel(int x, int y, CorRGBA cor) = 0;
    virtual void gravarPixel(int x, int y, uint32_t argb) = 0;
    virtual void gravarLinha(int y, int xInicio, int xFim, CorRGBA cor) = 0;
    virtual void desenharTexto(const std::string& texto, int x, int y, CorRGBA cor) = 0;
    virtual void apresentar() = 0;
    virtual int obterLargura() const = 0;
    virtual int obterAltura() const = 0;
    virtual bool estaAtivo() const = 0;
};
