#pragma once

#include <string>

#include "../../../entities/character/Character.h"

// TelaDerrota renderiza a interface quando o jogador cai em combate.
class TelaDerrota 
{
public:
    static void display(Character* currentPlayer, int goldEarned, int xpEarned, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns);
};
