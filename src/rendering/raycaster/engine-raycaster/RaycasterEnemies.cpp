#include "RaycasterEnemies.h"
#include "../../../core/state/EnemyCreator.h"
#include "../../../entities/races/RaceBase.h"
#include "../../../entities/character/Character.h"
#include <vector>
#include <string>
#include <memory>

namespace {
    std::vector<std::string> getArteFull(std::vector<std::unique_ptr<Character>> (*func)(int)) {
        auto vec = func(1);
        if (!vec.empty() && vec[0]) return vec[0]->obterRaca()->getRaceAppearance();
        return {"?"};
    }
}

void RaycasterInimigos::initializeSprites(std::map<char, SpriteCache>& cache) {
    cache['G'] = RaycasterSprites::parseSprite(getArteFull(EnemyCreator::createGoblinEnemy), 100, 200, 50); // Verde Goblin
    cache['O'] = RaycasterSprites::parseSprite(getArteFull(EnemyCreator::createExiledOrcEnemy), 50, 150, 50); // Verde Escuro Orc
    cache['S'] = RaycasterSprites::parseSprite(getArteFull(EnemyCreator::createSlimeEnemy), 50, 200, 255); // Ciano Slime
    cache['F'] = RaycasterSprites::parseSprite(getArteFull(EnemyCreator::createFairyEnemy), 255, 100, 200); // Rosa Fairy
    cache['A'] = RaycasterSprites::parseSprite(getArteFull(EnemyCreator::createForestAbominationEnemy), 139, 69, 19); // Marrom Abominacao
    cache['T'] = RaycasterSprites::parseSprite(getArteFull(EnemyCreator::createTrollEnemy), 150, 150, 160); // Cinza Troll
    cache['M'] = RaycasterSprites::parseSprite(getArteFull(EnemyCreator::createMimicEnemy), 200, 150, 50); // Dourado Mimic
    
    cache['H'] = RaycasterSprites::parseSprite(getArteFull(EnemyCreator::createMahoragaEnemy), 255, 255, 255, true); // Branco Mahoraga
}
