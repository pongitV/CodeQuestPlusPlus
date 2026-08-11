#pragma once
#include <vector>
#include <string>
#include <chrono>
#include "../../../entities/classes/ClassBase.h"



class RaycasterControls {
public:
    static char processarInputEControles(
        Character* jogador,
        float& jogadorX,
        float& jogadorY,
        float& anguloVisao,
        float& pitchOffset,
        float tempoDelta,
        float velocidadeMovimento,
        const std::vector<std::string>& matrizDoMapa,
        int ALTURA_TELA,
        float sensibilidadeX,
        float sensibilidadeY,
        bool& primeiraIteracaoMouse,
        int& outHitX,
        int& outHitY,
        bool& rodando,
        std::chrono::steady_clock::time_point& tp1,

        bool& isMoving,
        float& bobbingTime,
        float& bobbingAmplitude,
        int& bobbingOffset
    );

    // English Alias
    static char processInputAndControls(
        Character* player, float& playerX, float& playerY, float& viewAngle, float& pitchOffset,
        float deltaTime, float moveSpeed, const std::vector<std::string>& mapMatrix,
        int screenHeight, float sensX, float sensY, bool& firstMouseIter,
        int& outHitX, int& outHitY, bool& running, std::chrono::steady_clock::time_point& tp1,
        bool& isMoving, float& bobbingTime, float& bobbingAmplitude, int& bobbingOffset
    ) {
        return processarInputEControles(player, playerX, playerY, viewAngle, pitchOffset, deltaTime, moveSpeed, mapMatrix, screenHeight, sensX, sensY, firstMouseIter, outHitX, outHitY, running, tp1, isMoving, bobbingTime, bobbingAmplitude, bobbingOffset);
    }
};
