#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../../entities/character/Character.h"

class EnemyCreator
{
public:
    static std::vector<std::unique_ptr<Character>> createGoblinEnemy(int amount = 3);
    static std::vector<std::unique_ptr<Character>> createSlimeEnemy(int amount = 3);
    static std::vector<std::unique_ptr<Character>> createFairyEnemy(int amount = 5);
    static std::vector<std::unique_ptr<Character>> createExiledOrcEnemy(int amount = 1);
    static std::vector<std::unique_ptr<Character>> createForestAbominationEnemy(int amount = 1);
    static std::vector<std::unique_ptr<Character>> createTrollEnemy(int amount = 1);
    static std::vector<std::unique_ptr<Character>> createMimicEnemy(int amount = 1);
    static std::vector<std::unique_ptr<Character>> createMahoragaEnemy(int amount = 1);

private:
    template<typename RaceType, typename ClassType>
    static std::vector<std::unique_ptr<Character>> createGenericEnemies(int amount, int maxVariation = 10);
};
