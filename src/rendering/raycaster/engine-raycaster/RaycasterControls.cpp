#include "RaycasterControls.h"
#include "../../../core/utils/InputControl.h"
#include "../../../core/input/InputSystem.h"
#include "../../../ui/screens/pause/ScreenPause.h"
#include "../../../systems/inventory/InventoryCombat.h"
#include "../../../ui/screens/attributes/ScreenAttributes.h"
#include "../../../ui/screens/diary/ScreenDiary.h"
#include "../../../core/state/Debug.h"
#include "../../../core/d2d-context/D2DContext.h"
#include "../../../core/window/GameWindow.h"
#include "RaycasterWorld.h"
#include "RaycasterFrame.h"
#include "Raycaster.h"
#include <cmath>
#include <thread>

using namespace std;

char RaycasterControls::processarInputEControles(
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
) {
    int larguraMapa = matrizDoMapa.empty() ? 0 : matrizDoMapa[0].size();
    int alturaMapa = matrizDoMapa.size();
    
    float oldPlayerX = jogadorX;
    float oldPlayerY = jogadorY;
    int oldCellX = (int)jogadorX;
    int oldCellY = (int)jogadorY;

    bool keyDownW = InputSystem::IsKeyPressed('W');
    bool keyDownS = InputSystem::IsKeyPressed('S');
    bool keyDownA = InputSystem::IsKeyPressed('A');
    bool keyDownD = InputSystem::IsKeyPressed('D');

    int deltaMouseX = 0, deltaMouseY = 0;
    InputSystem::IsMouseMoving(deltaMouseX, deltaMouseY);

    static float s_smoothDeltaX = 0.0f;
    static float s_smoothDeltaY = 0.0f;
    if (primeiraIteracaoMouse) {
        primeiraIteracaoMouse = false;
        s_smoothDeltaX = 0.0f;
        s_smoothDeltaY = 0.0f;
    } else {
        s_smoothDeltaX = s_smoothDeltaX * 0.35f + (float)deltaMouseX * 0.65f;
        s_smoothDeltaY = s_smoothDeltaY * 0.35f + (float)deltaMouseY * 0.65f;
    }

    anguloVisao += s_smoothDeltaX * sensibilidadeX;
    pitchOffset -= s_smoothDeltaY * sensibilidadeY;
    if (pitchOffset > 2.0f) pitchOffset = 2.0f;
    if (pitchOffset < -2.0f) pitchOffset = -2.0f;

    // Movimento e Strafing (Com system de Sliding)
    float velEfetiva = Debug::isSpeedHackActive ? (velocidadeMovimento * 2.2f) : velocidadeMovimento;
    float moveX = 0.0f;
    float moveY = 0.0f;

    if (keyDownW) {
        isMoving = true;
        moveX += cosf(anguloVisao) * velEfetiva * tempoDelta;
        moveY += sinf(anguloVisao) * velEfetiva * tempoDelta;
    }
    if (keyDownS) {
        isMoving = true;
        moveX -= cosf(anguloVisao) * velEfetiva * tempoDelta;
        moveY -= sinf(anguloVisao) * velEfetiva * tempoDelta;
    }
    if (keyDownA) { // Strafe Esquerda (-90 graus)
        isMoving = true;
        moveX += cosf(anguloVisao - 1.5708f) * velEfetiva * tempoDelta;
        moveY += sinf(anguloVisao - 1.5708f) * velEfetiva * tempoDelta;
    }
    if (keyDownD) { // Strafe Direita (+90 graus)
        isMoving = true;
        moveX += cosf(anguloVisao + 1.5708f) * velEfetiva * tempoDelta;
        moveY += sinf(anguloVisao + 1.5708f) * velEfetiva * tempoDelta;
    }

    if (isMoving) {
        float novoX = jogadorX + moveX;
        float novoY = jogadorY + moveY;
        
        if (novoY >= 0 && novoY < alturaMapa && jogadorX >= 0 && jogadorX < larguraMapa) {
            if (RaycasterWorld::isWalkable((int)jogadorX, (int)novoY, matrizDoMapa)) jogadorY = novoY;
        }
        if (jogadorY >= 0 && jogadorY < alturaMapa && novoX >= 0 && novoX < larguraMapa) {
            if (RaycasterWorld::isWalkable((int)novoX, (int)jogadorY, matrizDoMapa)) jogadorX = novoX;
        }
    }

    // Efeito de Head Bobbing (Balanco da Camera)
    if (isMoving) {
        bobbingTime += tempoDelta * 12.0f;
        bobbingAmplitude += tempoDelta * 5.0f; // Aumenta a strength do passo
        if (bobbingAmplitude > 1.0f) bobbingAmplitude = 1.0f;
    } else {
        bobbingAmplitude -= tempoDelta * 5.0f; // Suaviza a parada em 0.2 segundos
        if (bobbingAmplitude < 0.0f) {
            bobbingAmplitude = 0.0f;
            bobbingTime = 0.0f;
        } else {
            bobbingTime += tempoDelta * 12.0f;
        }
    }
    bobbingOffset = (int)(sinf(bobbingTime) * bobbingAmplitude * (ALTURA_TELA * 0.01f));

    if (Debug::isDebugKey()) {
        GameWindow::mostrarCursor();
        ClipCursor(nullptr);
        Debug::displayDebugMenu(jogador);
        if (auto* win = D2DContext::window) {
            HWND h = win->obterHWND();
            RECT rc;
            GetWindowRect(h, &rc);
            ClipCursor(&rc);
        }
        GameWindow::ocultarCursor();
        primeiraIteracaoMouse = true;
    }

    static bool s_escPressionadoAnterior = false;
    bool escAtual = InputSystem::IsKeyPressed(VK_ESCAPE);
    if ((escAtual && !s_escPressionadoAnterior) || InputSystem::WasKeyPressed(VK_ESCAPE)) {
        s_escPressionadoAnterior = true;
        ClipCursor(nullptr);
        GameWindow::mostrarCursor();
        rodando = false;
        return 27;
    }
    if (!escAtual) s_escPressionadoAnterior = false;
    for (char k : {'M', 'I', 'F', 'B', 'C', 'J'}) {
        if (InputSystem::WasKeyPressed(k)) {
            ClipCursor(nullptr);
            GameWindow::mostrarCursor();
            rodando = false;
            return k;
        }
    }

    // Verifica se o jogador pisou em um trigger (Enemy ou Teleporte) para acionar a transicao de map/combat
    int newCellX = (int)jogadorX;
    int newCellY = (int)jogadorY;
    if (newCellX != oldCellX || newCellY != oldCellY) {
        char cell = matrizDoMapa[newCellY][newCellX];
        bool isLabel = RaycasterWorld::isMapLabel(newCellX, newCellY, matrizDoMapa);
        if (RaycasterWorld::isTeleport(cell) || (!isLabel && RaycasterWorld::isEntity(cell))) {
            outHitX = newCellX;
            outHitY = newCellY;
            jogadorX = oldPlayerX; // Retorna para a exata posicao anterior flutuante
            jogadorY = oldPlayerY;
            rodando = false; // Sai do loop 3D e devolve o controle pro map top-down processar o evento!
        }
    }

    return '\0';
}
