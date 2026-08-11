#pragma once

class Character;

class TelaPause {
public:
    static void display(Character* jogador);

    // English Alias
    static void show(Character* player) { display(player); }
};

using PauseScreen = TelaPause;
