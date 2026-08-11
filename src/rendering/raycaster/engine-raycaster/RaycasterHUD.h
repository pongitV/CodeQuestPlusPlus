#pragma once

#include <vector>
#include <string>
#include "../../../entities/character/Character.h"

#include "../../direct-2d/D2DRenderer.h"

enum class CorBanner {
    OURO,
    YELLOW,
    ORANGE,
    GREEN_CLARO,
    GREEN_ESCURO
};

#include "../../../core/utils/Color.h"
extern Color g_hudBrickColor;

class RaycasterHUD {
public:
    static void desenharProcedural(D2DRenderer& d2d, int screenWidth, int screenHeight, float jogadorX, float jogadorY, float anguloVisao, const std::vector<std::string>& matrizDoMapa, const std::string& tituloMapa, bool temaFloresta, Character* jogador);
    static void desenharOpcoesCombateProcedural(D2DRenderer& d2d, int screenWidth, int screenHeight, int opcaoSelecionada, const std::string& mensagemBanner = "", CorBanner corBanner = CorBanner::OURO, const std::string& mensagemBannerLinha2 = "");
    static void gatilharShakeVida();


private:

    static void desenharMinimapaProcedural(D2DRenderer& d2d, int screenWidth, int screenHeight, float jogadorX, float jogadorY, float anguloVisao, const std::vector<std::string>& matrizDoMapa, const std::string& tituloMapa, bool temaFloresta);
    static void desenharBarraStatusProcedural(D2DRenderer& d2d, int screenWidth, int screenHeight, Character* jogador);
    static void desenharControlesProcedural(D2DRenderer& d2d, int screenWidth, int screenHeight);
};