#include "Drops.h"
#include "../../entities/character/Character.h"
#include "../../systems/inventory/ItemFactory.h"
#include "../../ui/screens/combat/ScreenCombat.h"
#include "../utils/RandomGenerator.h"
#include "../../systems/progress/Diary.h"
#include "../utils/DialogFunctions.h"
#include "../../core/utils/Color.h"

void Drops::reportAndProcessXPGold(Character* player, int xpDrop, int goldDrop, int& totalGold, int& totalXp) 
{
    player->ganharXp(xpDrop);
    player->ganharOuro(goldDrop);
    totalXp += xpDrop;
    totalGold += goldDrop;
}

void Drops::reportItemDrop(const std::string& itemName, int amount) 
{
}

void Drops::giveAndProcessItem(Character* player, ItemID itemId, int amount, std::vector<std::string>& obtainedItems, int dropChance)
{
    if (amount <= 0) return;
    if (dropChance < 100 && !RandomGenerator::rollChance(dropChance)) return;

    std::string itemName = ItemFactory::getNameDeID(itemId);
    if (itemName.empty() || itemName == "Desconhecido") {
        auto temp = ItemFactory::criarItem(itemId);
        if (temp) itemName = temp->getNameItem();
    }
    for (int i = 0; i < amount; ++i) {
        auto createdItem = ItemFactory::criarItem(itemId);
        if (createdItem && i == 0) itemName = createdItem->getNameItem(); // Pega nome com cores/degrade caso tenha
        player->obterInventario()->adicionarItem(std::move(createdItem));
        obtainedItems.push_back(itemName);
    }
    Diary::instancia().registrarItem(itemName);
    reportItemDrop(itemName, amount);
}
