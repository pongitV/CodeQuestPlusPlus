#pragma once

#include <string>
#include <vector>

class Character;

class TelaBestiario
{
public:
    static void displayLista(Character* currentPlayer);

    static void displayFicha(Character* currentPlayer, const std::string& nomeInimigo, int indiceDescoberto, const std::vector<std::string>& descobertos);

    // English Aliases
    static void showList(Character* player) { displayLista(player); }
};

using BestiaryScreen = TelaBestiario;
