#include "Drops.h"
#include "../../entities/character/Character.h"
#include "../../systems/inventory/ItemFactory.h"
#include "../../ui/screens/combat/ScreenCombat.h"
#include "../utils/RandomGenerator.h"
#include "../../systems/progress/Diary.h"
#include "../utils/DialogFunctions.h"
#include "../../core/utils/Color.h"

void Drops::reportAndProcessXPGold(Character* jogador, int xpDrop, int ouroDrop, int& ouroTotal, int& xpTotal) 
{
    jogador->ganharXp(xpDrop);
    jogador->ganharOuro(ouroDrop);
    xpTotal += xpDrop;
    ouroTotal += ouroDrop;

}

void Drops::reportItemDrop(const std::string& nomeItem, int amount) 
{
}

void Drops::giveAndProcessItem(Character* jogador, ItemID itemId, int amount, std::vector<std::string>& itensObtidos, int chanceDeDrop)
{
    if (amount <= 0) return;
    if (chanceDeDrop < 100 && !RandomGenerator::rolarChance(chanceDeDrop)) return;

    std::string nomeItem = ItemFactory::getNameDeID(itemId);
    if (nomeItem.empty() || nomeItem == "Desconhecido") {
        auto temp = ItemFactory::criarItem(itemId);
        if (temp) nomeItem = temp->getNameItem();
    }
    for (int i = 0; i < amount; ++i) {
        auto itemCriado = ItemFactory::criarItem(itemId);
        if (itemCriado && i == 0) nomeItem = itemCriado->getNameItem(); // Pega nome com cores/degrade caso tenha
        jogador->obterInventario()->adicionarItem(std::move(itemCriado));
        itensObtidos.push_back(nomeItem);
    }
    Diary::instancia().registrarItem(nomeItem);
    reportItemDrop(nomeItem, amount);
}
