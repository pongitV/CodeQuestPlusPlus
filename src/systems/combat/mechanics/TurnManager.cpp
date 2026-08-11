#include "TurnManager.h"
#include "../../../entities/character/Character.h"
#include <algorithm>

int GerenciadorTurnos::calcularMaxDestrezaInimigos(const std::vector<std::unique_ptr<Character>>& enemies) {
    int maxDestreza = 0;
    for (const auto& inimigoPtr : enemies) {
        if (inimigoPtr->getDexterity() > maxDestreza) {
            maxDestreza = inimigoPtr->getDexterity();
        }
    }
    return maxDestreza;
}

bool GerenciadorTurnos::inimigosSaoMaisAgeis(Character* jogador, int maxDestrezaInimigos) {
    return maxDestrezaInimigos > jogador->getDexterity();
}

bool GerenciadorTurnos::inimigosTemDobroDeAgilidade(Character* jogador, int maxDestrezaInimigos) {
    return maxDestrezaInimigos > (jogador->getDexterity() * 2);
}

bool GerenciadorTurnos::jogadorTemTurnoExtraNoInicio(Character* jogador, int maxDestrezaInimigos) {
    return jogador->getDexterity() > (maxDestrezaInimigos * 2);
}
