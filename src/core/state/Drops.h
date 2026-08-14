#pragma once

#include <string>
#include <vector>

class Character;
enum class ItemID;

class Drops 
{
public:
    // Centraliza o cálculo e o processamento de XP e ouro de monstros
    static void reportAndProcessXPGold(Character* player, int xpDrop, int goldDrop, int& totalGold, int& totalXp);
    
    // Padroniza a mensagem do recebimento de um item 
    static void reportItemDrop(const std::string& itemName, int amount);

    // Delega a responsabilidade de conceder o item e processar a lista de drops
    static void giveAndProcessItem(Character* player, ItemID itemId, int amount, std::vector<std::string>& obtainedItems, int dropChance = 100);
};
