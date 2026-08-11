#pragma once

#include <string>
#include <vector>

#include "../../races/RaceBase.h"

class Goblin : public RaceBase
{
public:
    std::string getRaceName() const override;
    RaceType obterRaceType() const override { return RaceType::Goblin; }
    Attributes getRaceAttributes() const override;
    std::string getRaceAbilityName() const override;
    std::string getRaceAbilityDescription() const override;
    const std::vector<std::string>& getRaceAppearance() const override;

    BestiaryInfo obterBestiaryInfo() const override;

    std::vector<std::unique_ptr<Item>> getRaceEquipment() const override;
    void realizarDrops(Character* enemy, Character* currentPlayer, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal) override;
};
