#pragma once

#include <d2d1.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <wincodec.h>

#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "d2d1.lib")

class GerenciadorTexturas2D {
public:
    // Carrega ou recupera um ID2D1Bitmap do cache usando o caminho do asset
    static ID2D1Bitmap* obterTextura(ID2D1RenderTarget* rt, const std::string& caminhoAsset);

    // Desenha a imagem PNG nativamente no RenderTarget com a posicao, tamanho e opacidade especificados
    static void desenharImagem(
        ID2D1RenderTarget* rt, 
        const std::string& caminhoAsset, 
        float x, float y, 
        float largura, float altura, 
        float opacidade = 1.0f
    );

    // Desenha a imagem com filtro de cor/efeito de status (ex: congelado, sangramento, veneno)
    static void desenharImagemComEfeito(
        ID2D1RenderTarget* rt,
        const std::string& caminhoAsset,
        float x, float y,
        float largura, float altura,
        D2D1_COLOR_F tintCor,
        float opacidade = 1.0f
    );

    // Libera o cache de texturas (ex: no encerramento)
    static void limpar();

    // English Aliases
    static ID2D1Bitmap* getTexture(ID2D1RenderTarget* rt, const std::string& path) { return obterTextura(rt, path); }
    static void drawImage(ID2D1RenderTarget* rt, const std::string& path, float x, float y, float w, float h, float op = 1.0f) {
        desenharImagem(rt, path, x, y, w, h, op);
    }
    static void clear() { limpar(); }

private:
    static std::wstring resolverCaminhoWIC(const std::string& caminhoUtf8);

    static std::unordered_map<std::string, ID2D1Bitmap*> s_cacheTexturas;
    static std::unordered_set<std::string> s_caminhosInvalidos;
    static IWICImagingFactory* s_wicFactory;
};

using TextureManager2D = GerenciadorTexturas2D;
