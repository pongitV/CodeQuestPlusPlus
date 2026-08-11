#pragma once

#include <string>
#include <vector>

class Character;
enum class ItemID;

class Drops 
{
public:
    // Centraliza o calculo e as mensagens na screen para as recompensas dos monstros
    static void reportAndProcessXPGold(Character* jogador, int xpDrop, int ouroDrop, int& ouroTotal, int& xpTotal);
    
    // Padroniza a mensagem verde ou branca do recebimento de um item 
    static void reportItemDrop(const std::string& nomeItem, int amount);

    // Delega a responsabilidade de dar o item e processar a string no array (Aplicando DRY)
    static void giveAndProcessItem(Character* jogador, ItemID itemId, int amount, std::vector<std::string>& itensObtidos, int chanceDeDrop = 100);
};
