#pragma once

#include "../RaceBase.h"

class Elf : public RaceBase 
{
public:
    std::string getRaceName() const override;
    RaceType getRaceType() const override { return RaceType::Elf; }
    std::string getRaceSpritePath() const override { return "assets/races/elf.png"; }
    const std::vector<std::string>& getRaceAppearance() const override;
    Attributes getRaceAttributes() const override;

    std::string getRaceAbilityName() const override;
    std::string getRaceAbilityDescription() const override;
    int processOffensiveDamage(int baseDamage, Character* attacker) override;
};
