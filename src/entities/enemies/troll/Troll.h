#pragma once

#include "../../races/RaceBase.h"

class Troll : public RaceBase
{
public:
    std::string getRaceName() const override;
    RaceType obterRaceType() const override { return RaceType::Troll; }
    Attributes getRaceAttributes() const override;
    std::string getRaceAbilityName() const override;
    std::string getRaceAbilityDescription() const override;
    const std::vector<std::string>& getRaceAppearance() const override;
    std::vector<std::unique_ptr<Item>> getRaceEquipment() const override;
    
    BestiaryInfo obterBestiaryInfo() const override;
    
    void realizarDrops(Character* enemy, Character* currentPlayer, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal) override;
};
