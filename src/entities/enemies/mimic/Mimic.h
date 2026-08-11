#pragma once

#include "../../races/RaceBase.h"
#include <string>
#include <vector>
#include <memory>

class Mimic : public RaceBase
{
private:
    int ouroRoubadoTotal = 0;

public:
    std::string getRaceName() const override;
    RaceType obterRaceType() const override { return RaceType::Mimic; }
    const std::vector<std::string>& getRaceAppearance() const override;
    Attributes getRaceAttributes() const override;
    std::vector<std::unique_ptr<Item>> getRaceEquipment() const override;

    std::string getRaceAbilityName() const override;
    std::string getRaceAbilityDescription() const override;
    BestiaryInfo obterBestiaryInfo() const override;
    void aoCausarDano(Character* atacante, Character* alvo, int danoCausado) override;
    void realizarDrops(Character* enemy, Character* currentPlayer, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal) override;
};
