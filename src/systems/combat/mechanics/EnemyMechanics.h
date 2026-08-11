#pragma once
#include <vector>

class Character;

// MecanicasDoInimigo avalia a selecao de alvos da intelligence artificial em combate.
class MecanicasInimigo {
public:
    // Determina o alvo de um ataque inimigo com base em prioridade (lacaios > aliados > jogador)
    static Character* escolherAlvo(const std::vector<Character*>& aliadosVivos, Character* currentPlayer);

    static Character* selectTarget(const std::vector<Character*>& livingAllies, Character* currentPlayer) {
        return escolherAlvo(livingAllies, currentPlayer);
    }
};

using MecanicasDoInimigo = MecanicasInimigo;
using EnemyMechanics = MecanicasInimigo;
