#pragma once

#include <string>
#include <vector>

#include "../../races/RaceBase.h"

class ForestAbomination : public RaceBase
{
private:
    bool activelyHealing = false;

public:
    std::string getRaceName() const override;
    RaceType getRaceType() const override { return RaceType::ForestAbomination; }
    Attributes getRaceAttributes() const override;
    std::string getRaceAbilityName() const override;
    std::string getRaceAbilityDescription() const override;
    const std::vector<std::string>& getRaceAppearance() const override;

    BestiaryInfo getBestiaryInfo() const override;

    void onDealingDamage(Character* attacker, Character* target, int damageDealt) override;

    void performDrops(Character* enemy, Character* currentPlayer, std::vector<std::string>& obtainedItems, int& totalGold, int& totalXp) override;
};

using AbominacaoFloresta = ForestAbomination;
