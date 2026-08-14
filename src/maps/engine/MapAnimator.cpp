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

int MapAnimator::animateMapIntroduction(
    const std::string& /*mapTitle*/,
    const std::vector<std::string>& mapArt,
    int /*artWidth*/,
    const std::vector<std::string>& transitionArt,
    int /*transitionWidth*/,
    Color /*themeColor*/,
    const std::vector<std::string>& mapMatrix,
    int playerPosX,
    int playerPosY,
    const std::function<std::string(char, int, int)>& cellFormatter,
    bool animate,
    bool useBannerAnimation,
    const std::function<void()>& actionAfterArtFadeIn
) {
    if (MapController::is3DExplorationActive()) {
        RaycasterWorld::atualizarMapHash(mapMatrix);
        return 0; 
    }

    RaycasterWorld::atualizarMapHash(mapMatrix);

    int terminalWidth = 120;
    int terminalHeight = 40;

    if (!animate) {
        int initialMapLine = 0;
        MapRenderer::renderMap(mapMatrix, playerPosX, playerPosY, terminalWidth, terminalHeight, initialMapLine, cellFormatter);
        return initialMapLine;
    }

    if (actionAfterArtFadeIn) {
        actionAfterArtFadeIn();
    }
    
    int initialMapLine = 0;

    std::vector<std::string> bannerBase;
    if (useBannerAnimation) {
        if (!mapArt.empty()) {
            bannerBase = mapArt;
        } else if (!transitionArt.empty()) {
            bannerBase = transitionArt;
        }
    }

    if (bannerBase.empty() || !useBannerAnimation) {
        MapRenderer::renderMap(mapMatrix, playerPosX, playerPosY, terminalWidth, terminalHeight, initialMapLine, cellFormatter);
        return initialMapLine;
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

    int startX, endX;
    MapRenderer::calculateHorizontalCamera(terminalWidth, playerPosX, mapMatrix.empty() ? 0 : static_cast<int>(mapMatrix[0].length()), startX, endX);
    std::string mapLeftMargin = MapRenderer::calculateCenteredMargin(terminalWidth, endX - startX);
    
    std::string controlsText = "W,A,S,D: Mover | V: Visao | I: Inventory | C: Ficha | B: Diary | M: Map";
    std::string controlsLeftMargin = MapRenderer::calculateCenteredMargin(terminalWidth, controlsText.length());
    
    int startY, endY;
    MapRenderer::calculateVerticalCamera(terminalHeight, initialMapLine, playerPosY, static_cast<int>(mapMatrix.size()), startY, endY);
    
    std::vector<std::string> mapLinesCache;
    for (int y = startY; y < endY; y++) {
        std::string lineStr = mapLeftMargin;
        lineStr.reserve(mapLeftMargin.size() + (endX - startX) * 10);
        for (int x = startX; x < endX; x++) {
            char c = (x < static_cast<int>(mapMatrix[y].length())) ? mapMatrix[y][x] : ' ';
            lineStr += cellFormatter(c, x, y);
        }
        mapLinesCache.push_back(lineStr);
    }

    std::ostringstream initialMap;
    initialMap << controlsLeftMargin << controlsText << "\n\n";
    for (int i = 0; i < (int)mapLinesCache.size(); i++) {
        initialMap << mapLinesCache[i];
    }
    
    for (int frame = 1; frame <= 15; frame++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    if (!InputControl::teclaPressionada()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    InputControl::limparBuffer();
    
    MapRenderer::renderMap(mapMatrix, playerPosX, playerPosY, terminalWidth, terminalHeight, initialMapLine, cellFormatter);
    
    return initialMapLine;
}

void MapAnimator::animateFlashbang(int r, int g, int b) {
    (void)r;
    (void)g;
    (void)b;
}
