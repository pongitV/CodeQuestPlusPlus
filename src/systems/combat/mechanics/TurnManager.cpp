#include "TurnManager.h"
#include "../../../entities/character/Character.h"
#include <algorithm>

int TurnManager::calculateMaxEnemyDexterity(const std::vector<std::unique_ptr<Character>>& enemies) {
    int maxDexterity = 0;
    for (const auto& enemyPtr : enemies) {
        if (enemyPtr->getDexterity() > maxDexterity) {
            maxDexterity = enemyPtr->getDexterity();
        }
    }
    return maxDexterity;
}

bool TurnManager::areEnemiesFaster(Character* player, int maxEnemyDexterity) {
    return maxEnemyDexterity > player->getDexterity();
}

bool TurnManager::doEnemiesHaveDoubleAgility(Character* player, int maxEnemyDexterity) {
    return maxEnemyDexterity > (player->getDexterity() * 2);
}

bool TurnManager::doesPlayerHaveExtraTurnAtStart(Character* player, int maxEnemyDexterity) {
    return player->getDexterity() > (maxEnemyDexterity * 2);
}
