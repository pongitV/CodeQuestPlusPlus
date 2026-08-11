#pragma once

#include "../Item.h"
#include <string>

class MissionItem : public Item {
private:
    std::string nome;
public:
    MissionItem(const std::string& nome, int price = 500);
    std::string getNameItem() const override;
    TipoEquipamento obterTipo() const override;
    std::vector<std::string> obterDetalhesInspecao(Character* character = nullptr) const override;
};

std::unique_ptr<Item> fabricarMissionItem(ItemID id);
