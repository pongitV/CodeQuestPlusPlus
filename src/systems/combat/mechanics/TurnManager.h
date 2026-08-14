#pragma once
#include <vector>
#include <memory>

class Character;

// Gerenciador de ordem de turnos e limites de agilidade dos combatentes.
class TurnManager {
public:
    static int calculateMaxEnemyDexterity(const std::vector<std::unique_ptr<Character>>& enemies);
    
    // Verifica se os inimigos agem antes do jogador com base na comparacao de destreza
    static bool areEnemiesFaster(Character* player, int maxEnemyDexterity);
    
    // Verifica se os inimigos possuem o dobro da destreza do jogador
    static bool doEnemiesHaveDoubleAgility(Character* player, int maxEnemyDexterity);
    
    // Verifica se o jogador possui o dobro da destreza dos inimigos permitindo turno extra no inicio
    static bool doesPlayerHaveExtraTurnAtStart(Character* player, int maxEnemyDexterity);

    // Compatibilidade legada
    static int calcularMaxDestrezaInimigos(const std::vector<std::unique_ptr<Character>>& inimigos) {
        return calculateMaxEnemyDexterity(inimigos);
    }
    static bool inimigosSaoMaisAgeis(Character* jogador, int maxDestrezaInimigos) {
        return areEnemiesFaster(jogador, maxDestrezaInimigos);
    }
    static bool inimigosTemDobroDeAgilidade(Character* jogador, int maxDestrezaInimigos) {
        return doEnemiesHaveDoubleAgility(jogador, maxDestrezaInimigos);
    }
    static bool jogadorTemTurnoExtraNoInicio(Character* jogador, int maxDestrezaInimigos) {
        return doesPlayerHaveExtraTurnAtStart(jogador, maxDestrezaInimigos);
    }
};

using GerenciadorTurnos = TurnManager;
using GerenciadorDeTurnos = TurnManager;
