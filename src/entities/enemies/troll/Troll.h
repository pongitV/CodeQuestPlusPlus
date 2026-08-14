#pragma once

#include "../../races/RaceBase.h"

class Troll : public RaceBase
{
public:
    std::string getRaceName() const override;
    RaceType getRaceType() const override { return RaceType::Troll; }
    Attributes getRaceAttributes() const override;
    std::string getRaceAbilityName() const override;
    std::string getRaceAbilityDescription() const override;
    const std::vector<std::string>& getRaceAppearance() const override;
    std::vector<std::unique_ptr<Item>> getRaceEquipment() const override;
    
    BestiaryInfo getBestiaryInfo() const override;
    
    void performDrops(Character* enemy, Character* currentPlayer, std::vector<std::string>& obtainedItems, int& totalGold, int& totalXp) override;
};
