#pragma once

#include <vector>
#include <string>

// Estrutura com estatisticas acumuladas ao longo de um combate.
struct CombatStatistics {
    int goldEarned = 0;
    int xpEarned = 0;
    int totalDamageDealt = 0;
    int totalDamageTaken = 0;
    int totalHealingReceived = 0;
    int combatTurns = 1;
    std::vector<std::string> obtainedItems;
    std::vector<std::string> defeatedEnemies;
    int attemptedParries = 0;
    int effectiveParries = 0;
    int perfectParries = 0;
    int highestDamageDealt = 0;
    int consumedItems = 0;
    std::vector<std::string> newDiscoveries;

    // Campos legados para compatibilidade
    int& amountDeOuroObtido = goldEarned;
    int& amountDeXpObtido = xpEarned;
    std::vector<std::string>& itensObtidos = obtainedItems;
    std::vector<std::string>& inimigosDerrotados = defeatedEnemies;
    int& parriesTentados = attemptedParries;
    int& parriesEfetivos = effectiveParries;
    int& parriesPerfeitos = perfectParries;
    int& maiorDanoCausado = highestDamageDealt;
    int& itensConsumidos = consumedItems;
    std::vector<std::string>& novasDescobertas = newDiscoveries;
};

using CombatStatisticse = CombatStatistics;
using EstatisticasCombate = CombatStatistics;
