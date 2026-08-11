#pragma once

#include <string>
#include "../../../../entities/character/Character.h"

class TelaDerrotaRaycaster {
public:
    static void display(Character* currentPlayer, int amountDeOuroObtido, int amountDeXpObtido, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns);
};
