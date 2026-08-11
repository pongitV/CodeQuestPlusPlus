#pragma once

#include <unordered_map>
#include <string>

enum class KeybindingId {
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Attack,
    Inventory,
    CharacterSheet,
    Diary,
    Map
};

class KeybindingManager {
private:
    std::unordered_map<KeybindingId, char> bindings;

public:
    KeybindingManager();
    ~KeybindingManager() = default;

    static KeybindingManager& getInstance() {
        static KeybindingManager instance;
        return instance;
    }

    void registerBinding(KeybindingId id, char key) {
        bindings[id] = key;
    }

    char getKey(KeybindingId id) const {
        auto it = bindings.find(id);
        if (it != bindings.end()) return it->second;
        return '\0';
    }

    void setKey(KeybindingId id, char key) {
        bindings[id] = key;
    }
};
