#pragma once

#include <string>
#include <vector>

#include "../../races/RaceBase.h"

class AbominacaoFloresta : public RaceBase
{
private:
    bool curandoAtivamente = false;

public:
    std::string getRaceName() const override;
    RaceType obterRaceType() const override { return RaceType::AbominacaoFloresta; }
    Attributes getRaceAttributes() const override;
    std::string getRaceAbilityName() const override;
    std::string getRaceAbilityDescription() const override;
    const std::vector<std::string>& getRaceAppearance() const override;

    BestiaryInfo obterBestiaryInfo() const override;

    void aoCausarDano(Character* atacante, Character* alvo, int danoCausado) override;

    void realizarDrops(Character* enemy, Character* currentPlayer, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal) override;
};
