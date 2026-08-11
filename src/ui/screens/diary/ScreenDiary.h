#pragma once

#include <string>

class Character;

class TelaDiario {
public:
    static void display(Character* currentPlayer);

    // English Alias
    static void show(Character* player) { display(player); }
};

using DiaryScreen = TelaDiario;
