#include "KeybindingManager.h"

KeybindingManager::KeybindingManager() {
    bindings[KeybindingId::MoveUp] = 'W';
    bindings[KeybindingId::MoveDown] = 'S';
    bindings[KeybindingId::MoveLeft] = 'A';
    bindings[KeybindingId::MoveRight] = 'D';
    bindings[KeybindingId::Attack] = ' ';
    bindings[KeybindingId::Inventory] = 'I';
    bindings[KeybindingId::CharacterSheet] = 'C';
    bindings[KeybindingId::Diary] = 'B';
    bindings[KeybindingId::Map] = 'M';
}
