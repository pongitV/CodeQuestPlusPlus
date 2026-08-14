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

template<typename RaceType, typename ClassType>
std::vector<std::unique_ptr<Character>> EnemyCreator::createGenericEnemies(int amount, int maxVariation)
{
    std::vector<std::unique_ptr<Character>> horde;
    horde.reserve(amount); 

    for (auto i{0}; i < amount; ++i) 
    {
        auto race{std::make_unique<RaceType>()};
        auto raceName{race->getRaceName()};
        auto enemy = std::make_unique<Character>(
            raceName,
            std::move(race),
            std::make_unique<ClassType>()
        );

        // Aplica uma pequena variação nos atributos para que cada monstro da horda seja único
        int healthVariation = RandomGenerator::getInt(-maxVariation, maxVariation);
        enemy->obterAtributosFinais().health += (enemy->obterAtributosFinais().health * healthVariation) / 100;
        enemy->definirVida(enemy->obterAtributosFinais().health); // Sincroniza a vida atual com a nova vida máxima
        
        int strengthVariation = RandomGenerator::getInt(-maxVariation, maxVariation);
        enemy->obterAtributosFinais().strength += (enemy->obterAtributosFinais().strength * strengthVariation) / 100;
        
        int dexterityVariation = RandomGenerator::getInt(-maxVariation, maxVariation);
        enemy->obterAtributosFinais().dexterity += (enemy->obterAtributosFinais().dexterity * dexterityVariation) / 100;

        horde.push_back(std::move(enemy));
    }
    return horde;
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createGoblinEnemy(int amount)
{
    return createGenericEnemies<Goblin, ClassBaseEnemy>(amount, 10);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createSlimeEnemy(int amount)
{
    return createGenericEnemies<Slime, ClassBaseEnemy>(amount, 10);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createFairyEnemy(int amount)
{
    return createGenericEnemies<Fairy, ClassBaseEnemy>(amount, 10);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createExiledOrcEnemy(int amount)
{
    return createGenericEnemies<ExiledOrc, ClassBaseEnemy>(amount, 5);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createForestAbominationEnemy(int amount)
{
    return createGenericEnemies<ForestAbomination, ClassBaseEnemy>(amount, 5);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createTrollEnemy(int amount)
{
    return createGenericEnemies<Troll, ClassBaseEnemy>(amount, 5);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createMimicEnemy(int amount)
{
    return createGenericEnemies<Mimic, ClassBaseEnemy>(amount, 10);
}

std::vector<std::unique_ptr<Character>> EnemyCreator::createMahoragaEnemy(int amount)
{
    return createGenericEnemies<Mahoraga, ClassBaseEnemy>(amount, 5);
}
