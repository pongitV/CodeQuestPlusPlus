#pragma once

#include "../Item.h"
#include <string>

class ItemConsumivel : public Item
{
private:
    std::string nome;

public:
    ItemConsumivel(const std::string& nome, int price = 3);

    std::string getNameItem() const override;
    TipoEquipamento obterTipo() const override;
    std::vector<std::string> obterDetalhesInspecao(Character* character = nullptr) const override;
};

std::unique_ptr<Item> fabricarItemConsumivel(ItemID id);
