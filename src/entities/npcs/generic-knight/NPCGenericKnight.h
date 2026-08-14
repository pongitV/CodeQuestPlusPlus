#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "../../character/Character.h"

class NPCGenericKnight {
public:
    static std::unique_ptr<Character> createKnight(const std::string& name);
    static void interact(Character* currentPlayer, bool& trollDefeated, bool& invitationReceived, int terminalWidth, std::vector<std::string>& currentMapMatrix, bool explorationActive, const std::function<void()>& restoreScreen, char targetCell, int nextX, int nextY);

    // Aliases legados
    static std::unique_ptr<Character> criarCavaleiro(const std::string& nome) { return createKnight(nome); }
    static void interagir(Character* currentPlayer, bool& trollDerrotado, bool& conviteRecebido, int larguraDoTerminal, std::vector<std::string>& matrizDoMapaAtual, bool exploracaoEstaAtiva, const std::function<void()>& restaurarTela, char celulaDestino, int proximaPosicaoX, int proximaPosicaoY) {
        interact(currentPlayer, trollDerrotado, conviteRecebido, larguraDoTerminal, matrizDoMapaAtual, exploracaoEstaAtiva, restaurarTela, celulaDestino, proximaPosicaoX, proximaPosicaoY);
    }
};

using NPCCavaleiroGenerico = NPCGenericKnight;
