#include "ScreenDefeat.h"
#include "../../UIManager.h"

void TelaDerrota::display(Character* currentPlayer, int goldEarned, int xpEarned, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns)
{
    GerenciadorPerspectiva::obterDerrotaUI().display(currentPlayer, goldEarned, xpEarned, totalDamageDealt, totalDamageTaken, totalHealingReceived, combatTurns);
}
