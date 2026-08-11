#include "MapAnimator.h"
#include "../../core/utils/InputControl.h"
#include "MapRenderer.h"
#include "../control/MapController.h"
#include "../../rendering/raycaster/engine-raycaster/RaycasterWorld.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>
#include "../../core/utils/Color.h"

int MapAnimatora::animarIntroducaoMapa(
    const std::string& tituloDoMapa,
    const std::vector<std::string>& arteDoMapa,
    int /*larguraArte*/,
    const std::vector<std::string>& arteTransicao,
    int /*larguraTransicao*/,
    Color /*corTema*/,
    const std::vector<std::string>& matrizDoMapa,
    int posicaoXDoJogador,
    int posicaoYDoJogador,
    const std::function<std::string(char, int, int)>& formatadorCelula,
    bool animar,
    bool usarAnimacaoBanner,
    const std::function<void()>& acaoAposFadeInArte
) {
    if (MapControllera::isExploracao3DAtiva()) {
        RaycasterWorld::atualizarMapHash(matrizDoMapa);
        return 0; 
    }

    RaycasterWorld::atualizarMapHash(matrizDoMapa);

    int larguraTerminal = 120;
    int alturaTerminal = 40;

    if (!animar) {
        int linhaInicialMapa = 0;
        RenderizadorMapa::renderizarMapa(matrizDoMapa, posicaoXDoJogador, posicaoYDoJogador, larguraTerminal, alturaTerminal, linhaInicialMapa, formatadorCelula);
        return linhaInicialMapa;
    }


    if (acaoAposFadeInArte) {
        acaoAposFadeInArte();
    }
    
    int linhaInicialMapa = 0;

    std::vector<std::string> bannerBase;
    if (usarAnimacaoBanner) {
        if (!arteDoMapa.empty()) {
            bannerBase = arteDoMapa;
        } else if (!arteTransicao.empty()) {
            bannerBase = arteTransicao;
        }
    }

    if (bannerBase.empty() || !usarAnimacaoBanner) {
        RenderizadorMapa::renderizarMapa(matrizDoMapa, posicaoXDoJogador, posicaoYDoJogador, larguraTerminal, alturaTerminal, linhaInicialMapa, formatadorCelula);
        return linhaInicialMapa;
    }

    std::vector<std::string> banner;
    for (const auto& l : bannerBase) {
        banner.push_back(l);
    }
    
    int maxW = 0;
    for (const auto& l : banner) {
        int w = (int)(l).length();
        if (w > maxW) maxW = w;
    }
    
    int bannerHeight = banner.size();
    int startXBox = (larguraTerminal - maxW) / 2;
    if (startXBox < 0) startXBox = 0;

    int startX, endX;
    RenderizadorMapa::calcularCameraHorizontal(larguraTerminal, posicaoXDoJogador, matrizDoMapa.empty() ? 0 : static_cast<int>(matrizDoMapa[0].length()), startX, endX);
    std::string margemEsquerdaDoMapa = RenderizadorMapa::calcularMargemCentralizada(larguraTerminal, endX - startX);
    
    std::string textoDeControles = "W,A,S,D: Mover | V: Visao | I: Inventory | C: Ficha | B: Diary | M: Map";
    std::string margemEsquerdaControles = RenderizadorMapa::calcularMargemCentralizada(larguraTerminal, textoDeControles.length());
    
    int offsetMapaReal = 2;
    
    int startY, endY;
    RenderizadorMapa::calcularCameraVertical(alturaTerminal, linhaInicialMapa, posicaoYDoJogador, static_cast<int>(matrizDoMapa.size()), startY, endY);
    
    std::vector<std::string> linhasDoMapaCache;
    for (int y = startY; y < endY; y++) {
        std::string linhaStr = margemEsquerdaDoMapa;
        linhaStr.reserve(margemEsquerdaDoMapa.size() + (endX - startX) * 10);
        for (int x = startX; x < endX; x++) {
            char c = (x < static_cast<int>(matrizDoMapa[y].length())) ? matrizDoMapa[y][x] : ' ';
            linhaStr += formatadorCelula(c, x, y);
        }
        linhasDoMapaCache.push_back(linhaStr);
    }

    std::ostringstream initialMap;
    initialMap << margemEsquerdaControles << textoDeControles << "\n\n";
    for (int i = 0; i < (int)linhasDoMapaCache.size(); i++) {
        initialMap << linhasDoMapaCache[i];
    }

    int destinoY = 2;
    
    for (int frame = 1; frame <= 15; frame++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    if (!InputControl::teclaPressionada()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    InputControl::limparBuffer();
    
    RenderizadorMapa::renderizarMapa(matrizDoMapa, posicaoXDoJogador, posicaoYDoJogador, larguraTerminal, alturaTerminal, linhaInicialMapa, formatadorCelula);
    
    return linhaInicialMapa;
}

void MapAnimatora::animarFlashbang(int r, int g, int b) {
    (void)r;
    (void)g;
    (void)b;
}
