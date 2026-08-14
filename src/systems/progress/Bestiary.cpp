#include "Bestiary.h"
#include <algorithm>
#include "../../entities/character/Character.h"
#include "../../entities/enemies/goblin/Goblin.h"
#include "../../entities/enemies/slime/Slime.h"
#include "../../entities/enemies/fairy/Fairy.h"
#include "../../entities/enemies/exiled-orc/ExiledOrc.h"
#include "../../entities/enemies/forest-abomination/ForestAbomination.h"
#include "../../entities/enemies/troll/Troll.h"
#include "../../entities/enemies/mimic/Mimic.h"
#include "../../entities/enemies/mahoraga/Mahoraga.h"
#include "../../entities/enemies/ClassBaseEnemy.h"

Bestiary& Bestiary::instance() {
    static Bestiary inst;
    return inst;
}

Bestiary::Bestiary() {
    initializeEnemies();
}

namespace {
    template<typename T>
    void registerInBestiary(std::map<std::string, BestiaryEnemyInfo>& baseEnemies) {
        T race;
        ClassBaseInimigo defaultClass;
        Attributes attr = race.getRaceAttributes();
        BestiaryInfo info = race.getBestiaryInfo();
        
        std::vector<std::string> attrText = {
            " > Health           : " + std::to_string(attr.health),
            " > Forca          : " + std::to_string(attr.strength),
            " > Destreza       : " + std::to_string(attr.dexterity),
            " > Resistencia    : " + std::to_string(attr.resistance),
            " > Constituicao   : " + std::to_string(attr.constitution),
            " > Inteligencia   : " + std::to_string(attr.intelligence),
            " > Sabedoria      : " + std::to_string(attr.wisdom)
        };

        baseEnemies[race.getRaceName()] = {
            race.getRaceName(), info.map, info.habitat,
            race.getRaceAppearance(),
            info.lore,
            info.funFact,
            attrText,
            {defaultClass.getClassAbilityName() + " | " + defaultClass.getClassAbilityDescription()},
            race.getRaceAbilityName() + " | " + race.getRaceAbilityDescription(),
            info.drops,
            info.difficulty
        };
    }
}

void Bestiary::initializeEnemies() {
    registerInBestiary<Goblin>(baseEnemies);
    registerInBestiary<Slime>(baseEnemies);
    registerInBestiary<Fairy>(baseEnemies);
    registerInBestiary<OrkExilado>(baseEnemies);
    registerInBestiary<AbominacaoFloresta>(baseEnemies);
    registerInBestiary<Troll>(baseEnemies);
    registerInBestiary<Mimic>(baseEnemies);
    registerInBestiary<Mahoraga>(baseEnemies);
}

void Bestiary::registerFirstSight(const std::string& enemyName) {
    std::lock_guard<std::mutex> lock(mtx);
    if (baseEnemies.count(enemyName)) seenEnemies.insert(enemyName);
}

void Bestiary::registerDefeat(const std::string& enemyName) {
    std::lock_guard<std::mutex> lock(mtx);
    if (baseEnemies.count(enemyName)) {
        seenEnemies.insert(enemyName);
        defeatedEnemies.insert(enemyName);
        defeatCountMap[enemyName]++;
    }
}

void Bestiary::registerAbilitySeen(const std::string& enemyName, const std::string& ability) {
    std::lock_guard<std::mutex> lock(mtx);
    if (baseEnemies.count(enemyName)) seenAbilitiesMap[enemyName].insert(ability);
}

void Bestiary::registerDrop(const std::string& enemyName, const std::string& drop) {
    std::lock_guard<std::mutex> lock(mtx);
    if (baseEnemies.count(enemyName)) collectedDropsMap[enemyName].insert(drop);
}

bool Bestiary::isDiscovered(const std::string& enemyName) const {
    std::lock_guard<std::mutex> lock(mtx);
    return seenEnemies.count(enemyName) > 0;
}

bool Bestiary::isDefeated(const std::string& enemyName) const {
    std::lock_guard<std::mutex> lock(mtx);
    return defeatedEnemies.count(enemyName) > 0;
}

int Bestiary::getDefeatCount(const std::string& enemyName) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = defeatCountMap.find(enemyName);
    if (it != defeatCountMap.end()) {
        return it->second;
    }
    return 0;
}

bool Bestiary::hasSeenAbility(const std::string& enemyName, const std::string& ability) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = seenAbilitiesMap.find(enemyName);
    if (it != seenAbilitiesMap.end()) return it->second.count(ability) > 0;
    return false;
}

bool Bestiary::hasCollectedDrop(const std::string& enemyName, const std::string& drop) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = collectedDropsMap.find(enemyName);
    if (it != collectedDropsMap.end()) return it->second.count(drop) > 0;
    return false;
}

const BestiaryEnemyInfo* Bestiary::getInfo(const std::string& enemyName) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = baseEnemies.find(enemyName);
    if (it != baseEnemies.end()) return &it->second;
    return nullptr;
}

std::vector<std::string> Bestiary::getEnemiesOrderedByDifficulty() const {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::string> names;
    names.reserve(baseEnemies.size());
    for (const auto& pair : baseEnemies) names.push_back(pair.first);
    
    std::sort(names.begin(), names.end(), [this](const std::string& a, const std::string& b) {
        return baseEnemies.at(a).difficulty < baseEnemies.at(b).difficulty;
    });
    
    return names;
}

void Bestiary::save(std::ofstream& out) const {
    std::lock_guard<std::mutex> lock(mtx);
    
    auto writeSet = [&](const auto& set) {
        out << set.size() << "\n";
        for (const auto& item : set) out << item << "\n";
    };

    writeSet(seenEnemies);
    writeSet(defeatedEnemies);

    out << defeatCountMap.size() << "\n";
    for (const auto& [name, count] : defeatCountMap) out << name << "\n" << count << "\n";

    auto writeMapOfSets = [&](const auto& map) {
        out << map.size() << "\n";
        for (const auto& [name, set] : map) {
            out << name << "\n";
            writeSet(set);
        }
    };

    writeMapOfSets(seenAbilitiesMap);
    writeMapOfSets(collectedDropsMap);
}

void Bestiary::load(std::ifstream& in) {
    std::lock_guard<std::mutex> lock(mtx);
    seenEnemies.clear();
    defeatedEnemies.clear();
    defeatCountMap.clear();
    seenAbilitiesMap.clear();
    collectedDropsMap.clear();

    auto readSet = [&](auto& set) {
        size_t size;
        if (!(in >> size)) return false;
        std::string line; std::getline(in, line);
        for (size_t i = 0; i < size; ++i) {
            std::getline(in, line);
            set.insert(line);
        }
        return true;
    };
    
    if (!readSet(seenEnemies)) return; // Failsafe para saves antigos
    readSet(defeatedEnemies);
    
    size_t defeatCountSize;
    if (in >> defeatCountSize) {
        std::string line; std::getline(in, line);
        for (size_t i = 0; i < defeatCountSize; ++i) {
            std::string name; std::getline(in, name);
            int count; in >> count; std::getline(in, line);
            defeatCountMap[name] = count;
        }
    }

    auto readMap = [&](auto& map) {
        size_t size;
        if (!(in >> size)) return;
        std::string line; std::getline(in, line);
        for (size_t i = 0; i < size; ++i) {
            std::string key; std::getline(in, key);
            readSet(map[key]);
        }
    };

    readMap(seenAbilitiesMap);
    readMap(collectedDropsMap);
}






