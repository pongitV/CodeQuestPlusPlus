#pragma once

#include "../../races/RaceBase.h"
#include <string>
#include <vector>
#include <memory>

class Mimic : public RaceBase
{
private:
    int totalStolenGold = 0;

public:
    std::string getRaceName() const override;
    RaceType getRaceType() const override { return RaceType::Mimic; }
    const std::vector<std::string>& getRaceAppearance() const override;
    Attributes getRaceAttributes() const override;
    std::vector<std::unique_ptr<Item>> getRaceEquipment() const override;

    std::string getRaceAbilityName() const override;
    std::string getRaceAbilityDescription() const override;
    BestiaryInfo getBestiaryInfo() const override;
    void onDealingDamage(Character* attacker, Character* target, int damageDealt) override;
    void performDrops(Character* enemy, Character* currentPlayer, std::vector<std::string>& obtainedItems, int& totalGold, int& totalXp) override;
};
