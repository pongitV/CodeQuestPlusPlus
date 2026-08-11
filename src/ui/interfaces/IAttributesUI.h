#pragma once
#include "../../entities/character/Character.h"

class IAtributosUI {
public:
    virtual ~IAtributosUI() = default;
    virtual void display(Character* jogador) = 0;
    virtual void displayDetalhesAtributos(Character* currentPlayer) = 0;
    virtual void gerenciarFichaDoJogador(Character* currentPlayer) = 0;
};
