#pragma once
#include <vector>
#include <string>

class MapPhysics {
public:
    // Garante que a posicao nao saia dos limites da matriz do map
    static void applyMapBoundaries(int& posX, int& posY, const std::vector<std::string>& mapMatrix);
    static void aplicarLimitesDeMapa(int& posicaoX, int& posicaoY, const std::vector<std::string>& matrizDoMapa) {
        applyMapBoundaries(posicaoX, posicaoY, matrizDoMapa);
    }

    // Movimenta enemies na matriz respeitando limites e posicoes originais
    static void moveEnemiesRandomly(std::vector<std::string>& currentMapMatrix, const std::vector<std::string>& originalMatrix, const std::string& enemySymbols, int playerX, int playerY);
    static void moverInimigosAleatoriamente(std::vector<std::string>& matrizDoMapaAtual, const std::vector<std::string>& matrizOriginal, const std::string& simbolosInimigos, int jogadorX, int jogadorY) {
        moveEnemiesRandomly(matrizDoMapaAtual, matrizOriginal, simbolosInimigos, jogadorX, jogadorY);
    }
};

using MapPhysicsa = MapPhysics;
