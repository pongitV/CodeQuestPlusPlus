#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "../../entities/character/Character.h"

enum class MapType {
    Village,
    Forest,
    Kingdom,
    Labyrinth,

    // Compatibilidade legada
    Labirinto = Labyrinth
};

class StateEvent {
public:
    enum class Type { Enter, Exit, Update };
    Type type = Type::Update;
    std::string stateName;
    void* payload = nullptr;
};

class StateObserver {
public:
    virtual ~StateObserver() = default;
    virtual void onEvent(const StateEvent& event) = 0;
};

class MapManager {
private:
    std::unordered_map<MapType, std::vector<std::string>> mapCache;
    std::vector<std::shared_ptr<StateObserver>> observers;

public:
    MapManager() = default;
    ~MapManager() = default;

    static MapManager& getInstance() {
        static MapManager instance;
        return instance;
    }

    void cacheMap(MapType type, const std::vector<std::string>& matrix) {
        mapCache[type] = matrix;
    }

    bool hasMap(MapType type) const {
        return mapCache.find(type) != mapCache.end();
    }

    const std::vector<std::string>& getMap(MapType type) const {
        return mapCache.at(type);
    }

    void addObserver(std::shared_ptr<StateObserver> obs) {
        observers.push_back(obs);
    }

    void notifyObservers(const StateEvent& event) {
        for (auto& obs : observers) {
            if (obs) obs->onEvent(event);
        }
    }
};
