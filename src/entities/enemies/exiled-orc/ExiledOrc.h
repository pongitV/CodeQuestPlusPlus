#pragma once

#include <string>
#include <vector>

#include "../../races/orc/Orc.h"

class OrkExilado : public Ork
{
public:
    std::string getRaceName() const override;
    RaceType obterRaceType() const override { return RaceType::OrkExilado; }
    Attributes getRaceAttributes() const override;
    const std::vector<std::string>& getRaceAppearance() const override;
    std::vector<std::unique_ptr<Item>> getRaceEquipment() const override;

    BestiaryInfo obterBestiaryInfo() const override;

    void realizarDrops(Character* enemy, Character* currentPlayer, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal) override;
};
