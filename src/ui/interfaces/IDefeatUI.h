#pragma once
#include "../../entities/character/Character.h"

class IDefeatUI {
public:
    virtual ~IDefeatUI() = default;
    virtual void display(Character* player, int goldEarned, int xpEarned, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns) = 0;
};

using IDefeatUI = IDefeatUI;
