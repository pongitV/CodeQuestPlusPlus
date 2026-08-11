#pragma once

#include <vector>
#include <string>
#include <utility>

#include "../../../entities/character/Character.h"

// TelaVitoria renderiza o painel e estatisticas de vitoria do jogador.
class TelaVitoria 
{
public:
    static void display(Character* currentPlayer, int goldEarned, int xpEarned,
        int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns,
        const std::vector<std::string>& itensObtidos, const std::vector<std::string>& inimigosDerrotados,
        int parriesPerfeitos, int maiorDano, int parriesTentados, int parriesEfetivos, int itensConsumidos, const std::vector<std::string>& novasDescobertas);
};
