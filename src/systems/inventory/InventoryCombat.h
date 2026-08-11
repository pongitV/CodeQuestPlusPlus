#pragma once

class Character;
class Item;

class InventarioCombate
{
public:
    static void gerenciarInventario(Character* currentPlayer, bool* turnoFoiConsumido = nullptr);
};
