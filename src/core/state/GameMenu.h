#pragma once

#include <memory>

#include "../../entities/character/Character.h"

class GameMenu
{
public:
    static std::unique_ptr<Character> mainMenu();
    static std::unique_ptr<Character> startCharacterCreationSystem();

private:
};
