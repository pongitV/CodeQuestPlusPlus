#include "MapHelper.h"
#include "../../core/state/Debug.h"

bool MapHelper::isWall(char cell) {
    static const std::string walls = "*|_[]{}-=#WBPT";
    return cell != '\0' && walls.find(cell) != std::string::npos;
}

bool MapHelper::isEntityOrTeleport(char cell) {
    return cell == 'E' || cell == 'N' || cell == 'M' || cell == 'S';
}

bool MapHelper::checkCollision(float nextX, float nextY, const std::vector<std::string>& mapMatrix) {
    if (mapMatrix.empty()) return true;
    int gridY = static_cast<int>(nextY);
    int gridX = static_cast<int>(nextX);
    if (gridY < 0 || gridY >= static_cast<int>(mapMatrix.size())) return true;
    if (gridX < 0 || gridX >= static_cast<int>(mapMatrix[gridY].size())) return true;

    char cell = mapMatrix[gridY][gridX];
    return isWall(cell);
}

bool MapHelper::tryMove(Character* /*jogador*/, float deltaX, float deltaY, const std::vector<std::string>& mapMatrix, float& outNewX, float& outNewY) {
    float targetX = outNewX + deltaX;
    float targetY = outNewY + deltaY;

    bool collidedX = checkCollision(targetX, outNewY, mapMatrix);
    bool collidedY = checkCollision(outNewX, targetY, mapMatrix);

    if (!collidedX) outNewX = targetX;
    if (!collidedY) outNewY = targetY;

    return (!collidedX || !collidedY);
}

void MapHelper::processMovement(int& posX, int& posY, int targetX, int targetY, char targetCell, const std::string& obstacleChars) {
    bool isObstacle = isWall(targetCell);
    if (!obstacleChars.empty() && obstacleChars.find(targetCell) != std::string::npos) {
        isObstacle = true;
    }
    if (!isObstacle || Debug::isNoclipActive) {
        posX = targetX;
        posY = targetY;
    }
}
