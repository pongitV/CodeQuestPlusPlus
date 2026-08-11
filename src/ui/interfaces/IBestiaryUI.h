#pragma once
#include "../../entities/character/Character.h"
#include <vector>

class IBestiarioUI {
public:
    virtual ~IBestiarioUI() = default;
    virtual void display(const std::vector<Character*>& enemies) = 0;
    virtual void displayDetalhe(Character* enemy) = 0;
};
