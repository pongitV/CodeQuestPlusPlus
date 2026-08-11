#include "EnemyMechanics.h"
#include "../../../entities/character/Character.h"
#include "../../../core/utils/RandomGenerator.h"

Character* MecanicasInimigo::escolherAlvo(const std::vector<Character*>& aliadosVivos, Character* currentPlayer) {
    std::vector<Character*> alvosPossiveis;
    std::vector<Character*> minionsVivos;
    std::vector<Character*> aliadosNormaisVivos;

    for (auto* aliado : aliadosVivos) {
        if (aliado->isMinion()) {
            minionsVivos.push_back(aliado);
        } else {
            aliadosNormaisVivos.push_back(aliado);
        }
    }

    if (!minionsVivos.empty()) {
        alvosPossiveis = minionsVivos;
    } else if (!aliadosNormaisVivos.empty()) {
        alvosPossiveis = aliadosNormaisVivos;
    } else {
        alvosPossiveis.push_back(currentPlayer);
    }

    return alvosPossiveis[RandomGenerator::getInteiro(0, static_cast<int>(alvosPossiveis.size()) - 1)];
}
