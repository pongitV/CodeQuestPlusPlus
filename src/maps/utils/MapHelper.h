#pragma once

#include <vector>
#include <string>
#include "../../entities/character/Character.h"
#include "../../core/utils/InputControl.h"

// Classe utilitaria para centralizar rotinas comuns de mapas (colisao, transicao e movimentacao)
class MapHelper {
public:
    static bool checkCollision(float nextX, float nextY, const std::vector<std::string>& mapMatrix);
    static bool isWall(char cell);
    static bool isEntityOrTeleport(char cell);
    static bool tryMove(Character* player, float deltaX, float deltaY, const std::vector<std::string>& mapMatrix, float& outNewX, float& outNewY);
    static void processMovement(int& posX, int& posY, int targetX, int targetY, char targetCell, const std::string& obstacleChars = "");

    // Aliases legados
    static bool verificarColisao(float nextX, float nextY, const std::vector<std::string>& matrizDoMapa) { return checkCollision(nextX, nextY, matrizDoMapa); }
    static bool ehParede(char celula) { return isWall(celula); }
    static bool ehEntidadeOuTeleporte(char celula) { return isEntityOrTeleport(celula); }
    static bool tentarMover(Character* jogador, float deltaX, float deltaY, const std::vector<std::string>& matrizDoMapa, float& outNewX, float& outNewY) {
        return tryMove(jogador, deltaX, deltaY, matrizDoMapa, outNewX, outNewY);
    }
    static void processarMovimento(int& posX, int& posY, int targetX, int targetY, char celulaDestino, const std::string& caracteresObstaculo = "") {
        processMovement(posX, posY, targetX, targetY, celulaDestino, caracteresObstaculo);
    }
};
