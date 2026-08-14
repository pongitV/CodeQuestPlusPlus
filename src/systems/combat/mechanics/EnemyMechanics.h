#pragma once
#include <vector>

class Character;

// Avalia a selecao de alvos da intelligence artificial em combate.
class EnemyMechanics {
public:
    // Determina o alvo de um ataque inimigo com base em prioridade (lacaios > aliados > jogador)
    static Character* selectTarget(const std::vector<Character*>& livingAllies, Character* currentPlayer);

    // Compatibilidade legada
    static Character* escolherAlvo(const std::vector<Character*>& aliadosVivos, Character* currentPlayer) {
        return selectTarget(aliadosVivos, currentPlayer);
    }
};

using MecanicasInimigo = EnemyMechanics;
using MecanicasDoInimigo = EnemyMechanics;
