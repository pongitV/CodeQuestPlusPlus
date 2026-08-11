#pragma once
#include <vector>
#include <memory>

class Character;

// GerenciadorDeTurnos avalia a ordem de turnos e limites de agilidade dos combatentes.
class GerenciadorTurnos {
public:
    static int calcularMaxDestrezaInimigos(const std::vector<std::unique_ptr<Character>>& inimigos);
    
    // Verifica se os inimigos agem antes do jogador com base na comparacao de dexterity
    static bool inimigosSaoMaisAgeis(Character* jogador, int maxDestrezaInimigos);
    
    // Verifica se os inimigos possuem o dobro da dexterity do jogador
    static bool inimigosTemDobroDeAgilidade(Character* jogador, int maxDestrezaInimigos);
    
    // Verifica se o jogador possui o dobro da dexterity dos inimigos permitindo turno extra no inicio
    static bool jogadorTemTurnoExtraNoInicio(Character* jogador, int maxDestrezaInimigos);

    static int calculateMaxEnemyDexterity(const std::vector<std::unique_ptr<Character>>& enemies) {
        return calcularMaxDestrezaInimigos(enemies);
    }
    static bool areEnemiesFaster(Character* player, int maxEnemyDexterity) {
        return inimigosSaoMaisAgeis(player, maxEnemyDexterity);
    }
    static bool doEnemiesHaveDoubleAgility(Character* player, int maxEnemyDexterity) {
        return inimigosTemDobroDeAgilidade(player, maxEnemyDexterity);
    }
    static bool doesPlayerHaveExtraTurnAtStart(Character* player, int maxEnemyDexterity) {
        return jogadorTemTurnoExtraNoInicio(player, maxEnemyDexterity);
    }
};

using GerenciadorDeTurnos = GerenciadorTurnos;
using TurnManager = GerenciadorTurnos;
