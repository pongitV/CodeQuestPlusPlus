#pragma once

#include "../Item.h"
#include <string>
#include <vector>

class MaterialItem : public Item {
private:
    std::string nome;
public:
    MaterialItem(const std::string& nome, int price = 3);

    std::string getNameItem() const override;
    TipoEquipamento obterTipo() const override;
    std::vector<std::string> obterDetalhesInspecao(Character* character = nullptr) const override;
};

std::unique_ptr<Item> fabricarMaterialItem(ItemID id);
