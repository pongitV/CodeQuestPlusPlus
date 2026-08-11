#pragma once
#include <vector>
#include <string>
#include <functional>
#include "../../core/utils/Color.h"

class MapAnimatora {
public:
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
    );

    static void animarFlashbang(int r, int g, int b);
};
