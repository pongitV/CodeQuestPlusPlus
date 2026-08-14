#pragma once

class Character;
class Item;

class InventoryCombat
{
public:
    static void manageInventory(Character* currentPlayer, bool* turnConsumed = nullptr);
    static void gerenciarInventario(Character* currentPlayer, bool* turnoFoiConsumido = nullptr) {
        manageInventory(currentPlayer, turnoFoiConsumido);
    }
};

using InventarioCombate = InventoryCombat;
