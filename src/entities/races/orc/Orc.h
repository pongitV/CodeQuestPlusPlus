#pragma once

#include "../RaceBase.h"

class Ork : public RaceBase
{
public:
    std::string getRaceName() const override;
    RaceType obterRaceType() const override { return RaceType::Ork; }
    std::string getRaceSpritePath() const override { return "assets/races/orc.png"; }
    const std::vector<std::string>& getRaceAppearance() const override;
    Attributes getRaceAttributes() const override;

    std::string getRaceAbilityName() const override;
    std::string getRaceAbilityDescription() const override;
    int processOffensiveDamage(int baseDamage, Character* atacante) override;
};
