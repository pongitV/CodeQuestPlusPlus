#pragma once

#include <string>
#include <vector>

class Character;

class TelaBestiarioRaycaster {
public:
    static void display(const std::vector<Character*>& enemies);
    static void displayDetalhe(Character* enemy);
};
