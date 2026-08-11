#pragma once

#include <vector>
#include <string>
#include <atomic>
#include "RaycasterFrame.h"
#include "../../../entities/character/Character.h"
#include "../../../core/utils/Color.h"
class Raycaster : public RaycasterFrame {
public:
    static std::atomic<float> sensibilidadeX;
    static std::atomic<float> sensibilidadeY;

    static const std::vector<std::string>* s_mapaAtual;
    static float s_jogadorXAtual;
    static float s_jogadorYAtual;
    static float s_anguloAtual;
    static std::string s_tituloAtual;
    static Character* s_currentPlayer;
    static float s_tempoAbsolutoInicial;
    static std::chrono::steady_clock::time_point s_tpInicio;

    static char iniciarExploracao3D(const std::vector<std::string>& matrizDoMapa, float& jogadorX, float& jogadorY, float& anguloVisao, const std::string& tituloMapa, Character* jogador, int& outHitX, int& outHitY, int tipoAnimacaoEntrada = 0);
    static std::vector<std::string> desenharQuadroEstatico3D(const std::vector<std::string>& matrizDoMapa, float jogadorX, float jogadorY, float anguloVisao, const std::string& tituloMapa, Character* jogador, int alturaOverride = -1);
    
    static void renderizarFundoAtualizadoD2D();
    static void executarAnimacaoPortaAbrindo();

    // English Aliases
    static char start3DExploration(const std::vector<std::string>& map, float& px, float& py, float& angle, const std::string& title, Character* player, int& outHitX, int& outHitY, int anim = 0) {
        return iniciarExploracao3D(map, px, py, angle, title, player, outHitX, outHitY, anim);
    }
    static std::vector<std::string> drawStatic3DFrame(const std::vector<std::string>& map, float px, float py, float angle, const std::string& title, Character* player, int heightOverride = -1) {
        return desenharQuadroEstatico3D(map, px, py, angle, title, player, heightOverride);
    }
    static void renderUpdatedD2DBackground() { renderizarFundoAtualizadoD2D(); }
};
