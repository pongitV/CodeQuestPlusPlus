#include "MapHelper.h"

bool MapHelper::ehParede(char celula) {
    static const std::string paredes = "*|_[]{}-=#WBPT";
    return celula != '\0' && paredes.find(celula) != std::string::npos;
}

bool MapHelper::ehEntidadeOuTeleporte(char celula) {
    return celula == 'E' || celula == 'N' || celula == 'M' || celula == 'S';
}

bool MapHelper::verificarColisao(float nextX, float nextY, const std::vector<std::string>& matrizDoMapa) {
    if (matrizDoMapa.empty()) return true;
    int gridY = static_cast<int>(nextY);
    int gridX = static_cast<int>(nextX);
    if (gridY < 0 || gridY >= static_cast<int>(matrizDoMapa.size())) return true;
    if (gridX < 0 || gridX >= static_cast<int>(matrizDoMapa[gridY].size())) return true;

    char celula = matrizDoMapa[gridY][gridX];
    return ehParede(celula);
}

#include "../../core/state/Debug.h"

bool MapHelper::tentarMover(Character* /*jogador*/, float deltaX, float deltaY, const std::vector<std::string>& matrizDoMapa, float& outNewX, float& outNewY) {
    float targetX = outNewX + deltaX;
    float targetY = outNewY + deltaY;

    bool colidiuX = verificarColisao(targetX, outNewY, matrizDoMapa);
    bool colidiuY = verificarColisao(outNewX, targetY, matrizDoMapa);

    if (!colidiuX) outNewX = targetX;
    if (!colidiuY) outNewY = targetY;

    return (!colidiuX || !colidiuY);
}

void MapHelper::processarMovimento(int& posX, int& posY, int targetX, int targetY, char celulaDestino, const std::string& caracteresObstaculo) {
    bool ehObstaculo = ehParede(celulaDestino);
    if (!caracteresObstaculo.empty() && caracteresObstaculo.find(celulaDestino) != std::string::npos) {
        ehObstaculo = true;
    }
    if (!ehObstaculo || Debug::isNoclipActive) {
        posX = targetX;
        posY = targetY;
    }
}
