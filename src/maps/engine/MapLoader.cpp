#include "MapLoader.h"
#include "../../ui/UIManager.h"
#include "../control/MapController.h"

void MapLoader::enterSubMap(
    std::vector<std::string>& currentMapMatrix, std::vector<std::string>& savedMainMapMatrix,
    int& savedPosXBeforeSubMap, int& savedPosYBeforeSubMap,
    int& playerPosX, int& playerPosY, bool& isInsideSubMap,
    std::string& currentMapTitle, std::vector<std::string>& savedSubMapMatrix, bool& subMapVisited,
    const std::vector<std::string>& generatedSubMapMatrix, int initialSubMapPosX, int initialSubMapPosY, const std::string& subMapTitle, const std::function<void()>& restoreScreen)
{
    savedMainMapMatrix = currentMapMatrix;
    savedPosXBeforeSubMap = playerPosX;
    savedPosYBeforeSubMap = playerPosY;

    if (!subMapVisited) { currentMapMatrix = generatedSubMapMatrix; subMapVisited = true; } 
    else { currentMapMatrix = savedSubMapMatrix; }
    standardizeMapSize(currentMapMatrix);

    playerPosX = initialSubMapPosX;
    playerPosY = initialSubMapPosY;
    isInsideSubMap = true;
    currentMapTitle = subMapTitle;
    if (!PerspectiveManager::getInstance().is3DViewActive()) restoreScreen();
    else MapController::signal3DMapChange();
}

void MapLoader::standardizeMapSize(std::vector<std::string>& mapMatrix) {
    if (mapMatrix.empty()) return;
    size_t maxLen = 0;
    for (const auto& row : mapMatrix) {
        if (row.length() > maxLen) maxLen = row.length();
    }
    for (auto& row : mapMatrix) {
        if (row.length() < maxLen) {
            row.append(maxLen - row.length(), ' ');
        }
    }
}
