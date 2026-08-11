#pragma once

#include <vector>
#include <string>

// Estrutura com estatisticas acumuladas ao longo de um combate.
struct CombatStatisticse {
    int goldEarned = 0;
    int xpEarned = 0;
    int totalDamageDealt = 0;
    int totalDamageTaken = 0;
    int totalHealingReceived = 0;
    int combatTurns = 1;
    std::vector<std::string> itensObtidos;
    std::vector<std::string> inimigosDerrotados;
    int parriesTentados = 0;
    int parriesEfetivos = 0;
    int parriesPerfeitos = 0;
    int maiorDanoCausado = 0;
    int itensConsumidos = 0;
    std::vector<std::string> novasDescobertas;

    // Campos legados para compatibilidade
    int& amountDeOuroObtido = goldEarned;
    int& amountDeXpObtido = xpEarned;
};
