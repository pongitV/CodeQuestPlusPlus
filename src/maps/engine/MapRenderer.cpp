#include "MapRenderer.h"
#include <iostream>
#include <algorithm>

namespace {
    void calculateCameraAxis(int maxVisible, int playerPos, int mapSize, int& start, int& end) {
        start = 0;
        end = mapSize;

        if (end > maxVisible) {
            start = std::max(0, playerPos - (maxVisible / 2));
            end = start + maxVisible;
            if (end > mapSize) {
                end = mapSize;
                start = std::max(0, end - maxVisible);
            }
        }
    }
}

void MapRenderer::calculateVerticalCamera(int screenHeight, int startLine, int playerPosY, int mapSize, int& startY, int& endY) {
    int maxVisibleLines = std::max(5, screenHeight - startLine - 4);
    calculateCameraAxis(maxVisibleLines, playerPosY, mapSize, startY, endY);
}

void MapRenderer::calculateHorizontalCamera(int screenWidth, int playerPosX, int mapWidth, int& startX, int& endX) {
    int maxVisibleCols = std::max(10, screenWidth); // Usa a largura total do terminal
    calculateCameraAxis(maxVisibleCols, playerPosX, mapWidth, startX, endX);
}

std::string MapRenderer::calculateCenteredMargin(int screenWidth, int textWidth) {
    int spaces = (screenWidth - textWidth) / 2;
    return std::string(spaces > 0 ? spaces : 0, ' ');
}

void MapRenderer::renderMap(const std::vector<std::string>& mapMatrix, int playerPosX, int playerPosY, int screenWidth, int screenHeight, int startLine, const std::function<std::string(char, int, int)>& cellFormatter) {
    static CameraCache cache;
    int startX, endX, startY, endY;

    if (cache.isValid(playerPosX, playerPosY, screenWidth, screenHeight)) {
        startX = cache.startX;
        endX = cache.endX;
        startY = cache.startY;
        endY = cache.endY;
    } else {
        calculateHorizontalCamera(screenWidth, playerPosX, mapMatrix.empty() ? 0 : static_cast<int>(mapMatrix[0].length()), startX, endX);
        calculateVerticalCamera(screenHeight, startLine, playerPosY, static_cast<int>(mapMatrix.size()), startY, endY);
        cache.lastPosX = playerPosX;
        cache.lastPosY = playerPosY;
        cache.lastTermW = screenWidth;
        cache.lastTermH = screenHeight;
        cache.startX = startX; cache.endX = endX;
        cache.startY = startY; cache.endY = endY;
    }

    std::string mapLeftMargin = calculateCenteredMargin(screenWidth, endX - startX);

    for (int y = startY; y < endY; y++) {
        std::string renderedLine = mapLeftMargin;
        renderedLine.reserve(mapLeftMargin.size() + (endX - startX) * 10);
        for (int x = startX; x < endX; x++) {
            char c = (x < static_cast<int>(mapMatrix[y].length())) ? mapMatrix[y][x] : ' ';
            renderedLine += cellFormatter(c, x, y);
        }
    }
}
