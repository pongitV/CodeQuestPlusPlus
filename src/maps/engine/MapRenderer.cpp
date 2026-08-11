#include "MapRenderer.h"
#include <iostream>
#include <algorithm>

namespace {
    void calcularCameraAxis(int maxVisivel, int posicaoJogador, int tamanhoMapa, int& start, int& end) {
        start = 0;
        end = tamanhoMapa;

        if (end > maxVisivel) {
            start = std::max(0, posicaoJogador - (maxVisivel / 2));
            end = start + maxVisivel;
            if (end > tamanhoMapa) {
                end = tamanhoMapa;
                start = std::max(0, end - maxVisivel);
            }
        }
    }
}

void RenderizadorMapa::calcularCameraVertical(int alturaDaTela, int linhaInicial, int posicaoYDoJogador, int tamanhoDoMapa, int& startY, int& endY) {
    int maxLinhasVisiveis = std::max(5, alturaDaTela - linhaInicial - 4);
    calcularCameraAxis(maxLinhasVisiveis, posicaoYDoJogador, tamanhoDoMapa, startY, endY);
}

void RenderizadorMapa::calcularCameraHorizontal(int larguraDaTela, int posicaoXDoJogador, int larguraDoMapa, int& startX, int& endX) {
    int maxColunasVisiveis = std::max(10, larguraDaTela); // Usa a largura total do terminal
    calcularCameraAxis(maxColunasVisiveis, posicaoXDoJogador, larguraDoMapa, startX, endX);
}

std::string RenderizadorMapa::calcularMargemCentralizada(int larguraDaTela, int larguraDoTexto) {
    int espacos = (larguraDaTela - larguraDoTexto) / 2;
    return std::string(espacos > 0 ? espacos : 0, ' ');
}

void RenderizadorMapa::renderizarMapa(const std::vector<std::string>& matrizDoMapa, int posicaoXDoJogador, int posicaoYDoJogador, int larguraDaTela, int alturaDaTela, int linhaInicial, const std::function<std::string(char, int, int)>& formatadorCelula) {
    static CameraCache cache;
    int startX, endX, startY, endY;

    if (cache.estaValido(posicaoXDoJogador, posicaoYDoJogador, larguraDaTela, alturaDaTela)) {
        startX = cache.startX;
        endX = cache.endX;
        startY = cache.startY;
        endY = cache.endY;
    } else {
        calcularCameraHorizontal(larguraDaTela, posicaoXDoJogador, matrizDoMapa.empty() ? 0 : static_cast<int>(matrizDoMapa[0].length()), startX, endX);
        calcularCameraVertical(alturaDaTela, linhaInicial, posicaoYDoJogador, static_cast<int>(matrizDoMapa.size()), startY, endY);
        cache.lastPosX = posicaoXDoJogador;
        cache.lastPosY = posicaoYDoJogador;
        cache.lastTermW = larguraDaTela;
        cache.lastTermH = alturaDaTela;
        cache.startX = startX; cache.endX = endX;
        cache.startY = startY; cache.endY = endY;
    }

    std::string margemEsquerdaDoMapa = calcularMargemCentralizada(larguraDaTela, endX - startX);

    for (int y = startY; y < endY; y++) {
        std::string linhaSendoRenderizada = margemEsquerdaDoMapa;
        linhaSendoRenderizada.reserve(margemEsquerdaDoMapa.size() + (endX - startX) * 10);
        for (int x = startX; x < endX; x++) {
            char c = (x < static_cast<int>(matrizDoMapa[y].length())) ? matrizDoMapa[y][x] : ' ';
            linhaSendoRenderizada += formatadorCelula(c, x, y);
        }
    }
}
