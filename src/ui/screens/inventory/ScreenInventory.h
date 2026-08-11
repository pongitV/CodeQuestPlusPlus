#pragma once

#include "../../../entities/character/Character.h"
#include <utility>
#include <vector>
#include <string>

class TelaInventario 
{
public:
    static void displayCaixaEquipados(Character* currentPlayer);
    static std::vector<std::pair<std::string, Item*>> obterListaCategoria(Character* currentPlayer, int categoria, bool mostrarPrecos = false);
    static void displayInspecaoItem(Item* item, Character* currentPlayer = nullptr);
    static void displayCabecalhoInventario(bool animar = false, int startY = -1);

    // English Aliases
    static void showEquippedBox(Character* player) { displayCaixaEquipados(player); }
    static void showItemInspection(Item* item, Character* player = nullptr) { displayInspecaoItem(item, player); }
};

using InventoryScreen = TelaInventario;
