#pragma once
#include <vector>
#include <string>
#include <functional>
#include "../../core/utils/Color.h"

class MapAnimator {
public:
    static int animateMapIntroduction(
        const std::string& mapTitle,
        const std::vector<std::string>& mapArt,
        int artWidth,
        const std::vector<std::string>& transitionArt,
        int transitionWidth,
        Color themeColor,
        const std::vector<std::string>& mapMatrix,
        int playerPosX,
        int playerPosY,
        const std::function<std::string(char, int, int)>& cellFormatter,
        bool animate,
        bool useBannerAnimation,
        const std::function<void()>& actionAfterArtFadeIn
    );

    static void animateFlashbang(int r, int g, int b);

    // Aliases legados
    static int animarIntroducaoMapa(
        const std::string& tituloDoMapa,
        const std::vector<std::string>& arteDoMapa,
        int larguraArte,
        const std::vector<std::string>& arteTransicao,
        int larguraTransicao,
        Color corTema,
        const std::vector<std::string>& matrizDoMapa,
        int posicaoXDoJogador,
        int posicaoYDoJogador,
        const std::function<std::string(char, int, int)>& formatadorCelula,
        bool animar,
        bool usarAnimacaoBanner,
        const std::function<void()>& acaoAposFadeInArte
    ) {
        return animateMapIntroduction(tituloDoMapa, arteDoMapa, larguraArte, arteTransicao, larguraTransicao, corTema, matrizDoMapa, posicaoXDoJogador, posicaoYDoJogador, formatadorCelula, animar, usarAnimacaoBanner, acaoAposFadeInArte);
    }

    static void animarFlashbang(int r, int g, int b) {
        animateFlashbang(r, g, b);
    }
};

using MapAnimatora = MapAnimator;
