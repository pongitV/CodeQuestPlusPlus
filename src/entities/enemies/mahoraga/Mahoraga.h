#pragma once

#include "../../races/RaceBase.h"

class Mahoraga : public RaceBase {
private:
    int parrysSofridos = 0;
    int defesasComEscudoSofridas = 0;
public:
    std::string getRaceName() const override;
    RaceType obterRaceType() const override { return RaceType::Mahoraga; }
    Attributes getRaceAttributes() const override;
    std::vector<std::unique_ptr<Item>> getRaceEquipment() const override;
    const std::vector<std::string>& getRaceAppearance() const override;

    std::string getRaceAbilityName() const override;
    std::string getRaceAbilityDescription() const override;
    
    void aoCausarDano(Character* atacante, Character* alvo, int danoCausado) override;
    void aoSofrerParryPerfeito() override;
    void aoTerAtaqueBloqueadoPorEscudo();
    bool ignoraParry() const override;
    bool ignoraEscudo() const override;

    BestiaryInfo obterBestiaryInfo() const override;
    void realizarDrops(Character* enemy, Character* currentPlayer, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal) override;
};
