#pragma once

#include "../../../entities/character/Character.h"

struct PoderCombate {
    int danoFisEst;
    int danoMagEst;
    int defFixa;
    double mitigacao;
};

struct DebuffInfo {
    int strengthPerdida;
    int dexterityPerdida;
    int resPerdida;
    int constPerdida;
    bool temBuff;
};

class TelaAtributos 
{
public:
    static void display(Character* currentPlayer);
    static void gerenciarFichaDoJogador(Character* currentPlayer);

    // English Aliases
    static void show(Character* player) { display(player); }
    static void managePlayerSheet(Character* player) { gerenciarFichaDoJogador(player); }

    static PoderCombate calcularPoderCombate(Character* currentPlayer, double multiplicador);
    static DebuffInfo calcularDebuff(Character* currentPlayer);
};

using AttributesScreen = TelaAtributos;
using CombatPower = PoderCombate;
