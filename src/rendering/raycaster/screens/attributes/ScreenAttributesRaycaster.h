#pragma once

#include <string>

class Character;

class TelaAtributosRaycaster {
public:
    static void display(Character* jogador);
    static void displayDetalhesAtributos(Character* currentPlayer);
    static void gerenciarFichaDoJogador(Character* currentPlayer);
};
