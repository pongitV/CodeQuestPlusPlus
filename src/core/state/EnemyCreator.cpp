#include "EnemyCreator.h"
#include <type_traits>
#include "../../entities/races/orc/Orc.h"
#include "../../entities/classes/ClassBase.h"
#include "../../systems/inventory/Item.h"
#include "../../systems/inventory/equipment/WeaponEquipment.h"
#include "../../systems/inventory/equipment/ArmorEquipment.h"
#include "../../entities/enemies/ClassBaseEnemy.h"
#include "../../entities/enemies/goblin/Goblin.h"
#include "../../entities/enemies/slime/Slime.h"
#include "../../entities/enemies/fairy/Fairy.h"
#include "../../entities/enemies/exiled-orc/ExiledOrc.h"
#include "../../entities/enemies/forest-abomination/ForestAbomination.h"
#include "../../entities/enemies/mimic/Mimic.h"
#include "../../entities/enemies/troll/Troll.h"
#include "../../entities/enemies/mahoraga/Mahoraga.h"
#include "../utils/RandomGenerator.h"

template<typename RacaType, typename ClasseType>
std::vector<std::unique_ptr<Character>> EnemyCreator::createGenericEnemies(int amount, int maxVariation)
{
    std::vector<std::unique_ptr<Character>> horda;
    horda.reserve(amount); 

    for (auto i{0}; i < amount; ++i) 
    {
        auto race{std::make_unique<RacaType>()};
        auto nomeRaca{race->getRaceName()};
        auto enemy = std::make_unique<Character>(
            nomeRaca,
            std::move(race),
            std::make_unique<ClasseType>()
        );

        // Aplica uma pequena variacao nos attributes para que cada monstro da horda seja unico
        int variacaoVida = RandomGenerator::getInteiro(-maxVariation, maxVariation);
        enemy->obterAtributosFinais().health += (enemy->obterAtributosFinais().health * variacaoVida) / 100;
        enemy->definirVida(enemy->obterAtributosFinais().health); // Sincroniza a health atual com a nova health maxima
        
        int variacaoForca = RandomGenerator::getInteiro(-maxVariation, maxVariation);
        enemy->obterAtributosFinais().strength += (enemy->obterAtributosFinais().strength * variacaoForca) / 100;
        
        int variacaoDestreza = RandomGenerator::getInteiro(-maxVariation, maxVariation);
        enemy->obterAtributosFinais().dexterity += (enemy->obterAtributosFinais().dexterity * variacaoDestreza) / 100;

        horda.push_back(std::move(enemy));
    }
    return horda;
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createGoblinEnemy(int amount)
{
    return createGenericEnemies<Goblin, ClassBaseInimigo>(amount, 10);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createSlimeEnemy(int amount)
{
    return createGenericEnemies<Slime, ClassBaseInimigo>(amount, 10);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createFairyEnemy(int amount)
{
    return createGenericEnemies<Fairy, ClassBaseInimigo>(amount, 10);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createExiledOrcEnemy(int amount)
{
    return createGenericEnemies<OrkExilado, ClassBaseInimigo>(amount, 5);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createForestAbominationEnemy(int amount)
{
    return createGenericEnemies<AbominacaoFloresta, ClassBaseInimigo>(amount, 5);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createTrollEnemy(int amount)
{
    return createGenericEnemies<Troll, ClassBaseInimigo>(amount, 5);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createMimicEnemy(int amount)
{
    return createGenericEnemies<Mimic, ClassBaseInimigo>(amount, 10);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createMahoragaEnemy(int amount)
{
    return createGenericEnemies<Mahoraga, ClassBaseInimigo>(amount, 5);
}
