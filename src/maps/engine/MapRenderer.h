#pragma once
#include <vector>
#include <string>
#include <functional>

struct CameraCache {
    int lastPosX = -1, lastPosY = -1;
    int lastTermW = -1, lastTermH = -1;
    int startX = 0, endX = 0;
    int startY = 0, endY = 0;

    bool estaValido(int posX, int posY, int termW, int termH) const {
        return (lastPosX == posX && lastPosY == posY && lastTermW == termW && lastTermH == termH);
    }
};

class RenderizadorMapa {
public:
    static void renderizarMapa(
        const std::vector<std::string>& matrizDoMapa, int posicaoXDoJogador, int posicaoYDoJogador, 
        int larguraDaTela, int alturaDaTela, int linhaInicial, 
        const std::function<std::string(char, int, int)>& formatadorCelula);

    static void calcularCameraVertical(int alturaDaTela, int linhaInicial, int posicaoYDoJogador, int tamanhoDoMapa, int& startY, int& endY);
    static void calcularCameraHorizontal(int larguraDaTela, int posicaoXDoJogador, int larguraDoMapa, int& startX, int& endX);
    static std::string calcularMargemCentralizada(int larguraDaTela, int larguraDoTexto);
};

struct RenderBatch {
    std::vector<std::pair<int, int>> cellPositions;
    std::vector<char> cellChars;
    std::vector<std::string> cellFormatted;

    void clear() {
        cellPositions.clear();
        cellChars.clear();
        cellFormatted.clear();
    }
};
