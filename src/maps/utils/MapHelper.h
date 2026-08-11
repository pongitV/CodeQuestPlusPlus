#pragma once

#include <vector>
#include <string>
#include "../../entities/character/Character.h"
#include "../../core/utils/InputControl.h"

// Classe utilitária para centralizar rotinas comuns de mapas (colisão, transição e movimentação)
class MapHelper {
public:
    static bool verificarColisao(float nextX, float nextY, const std::vector<std::string>& matrizDoMapa);
    static bool ehParede(char celula);
    static bool ehEntidadeOuTeleporte(char celula);
    static bool tentarMover(Character* jogador, float deltaX, float deltaY, const std::vector<std::string>& matrizDoMapa, float& outNewX, float& outNewY);
    static void processarMovimento(int& posX, int& posY, int targetX, int targetY, char celulaDestino, const std::string& caracteresObstaculo = "");
};
