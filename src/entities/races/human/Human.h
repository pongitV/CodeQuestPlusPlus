#pragma once

#include "../RaceBase.h"

class Human : public RaceBase 
{
public:
    std::string getRaceName() const override;
    RaceType obterRaceType() const override { return RaceType::Human; }
    std::string getRaceSpritePath() const override { return "assets/races/human.png"; }
    const std::vector<std::string>& getRaceAppearance() const override;
    Attributes getRaceAttributes() const override;

    std::string getRaceAbilityName() const override;
    std::string getRaceAbilityDescription() const override;
    int processDefensiveDamage(int finalDamage, Character* defensor) override;
};
