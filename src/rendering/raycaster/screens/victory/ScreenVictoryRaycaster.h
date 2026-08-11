#pragma once

#include <string>
#include <vector>
#include <utility>

class Character;

// TelaVitoriaRaycaster exibe a vitoria em modo direct-2d/Raycaster.
class TelaVitoriaRaycaster {
public:
    static void display(Character* currentPlayer, int goldEarned, int xpEarned,
        int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns,
        const std::vector<std::string>& inimigosDerrotados, int parriesPerfeitos, int maiorDano,
        int parriesTentados, int parriesEfetivos, int itensConsumidos, const std::vector<std::pair<std::string, int>>& dropsUnicos,
        bool podeSubirNivel, const std::vector<std::string>& novasDescobertas,
        const std::string& tituloMapa);
};
