#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "../../character/Character.h"

class NPCGenericKnight {
public:
    static std::unique_ptr<Character> criarCavaleiro(const std::string& nome);
    static void interagir(Character* currentPlayer, bool& trollDerrotado, bool& conviteRecebido, int larguraDoTerminal, std::vector<std::string>& matrizDoMapaAtual, bool exploracaoEstaAtiva, const std::function<void()>& restaurarTela, char celulaDestino, int proximaPosicaoX, int proximaPosicaoY);

    // English Aliases
    static std::unique_ptr<Character> createKnight(const std::string& name) { return criarCavaleiro(name); }
    static void interact(Character* currentPlayer, bool& trollDefeated, bool& invitationReceived, int terminalWidth, std::vector<std::string>& currentMapMatrix, bool explorationActive, const std::function<void()>& restoreScreen, char targetCell, int nextX, int nextY) {
        interagir(currentPlayer, trollDefeated, invitationReceived, terminalWidth, currentMapMatrix, explorationActive, restoreScreen, targetCell, nextX, nextY);
    }
};

using NPCGenericKnight = NPCGenericKnight;
