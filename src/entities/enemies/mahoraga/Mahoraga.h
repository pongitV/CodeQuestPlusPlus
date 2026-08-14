#pragma once

#include "../../races/RaceBase.h"

class Mahoraga : public RaceBase {
private:
    int sufferedParries = 0;
    int sufferedShieldDefenses = 0;
public:
    std::string getRaceName() const override;
    RaceType getRaceType() const override { return RaceType::Mahoraga; }
    Attributes getRaceAttributes() const override;
    std::vector<std::unique_ptr<Item>> getRaceEquipment() const override;
    const std::vector<std::string>& getRaceAppearance() const override;

    std::string getRaceAbilityName() const override;
    std::string getRaceAbilityDescription() const override;
    
    void onDealingDamage(Character* attacker, Character* target, int damageDealt) override;
    void onSufferingPerfectParry() override;
    void onAttackBlockedByShield();
    bool ignoresParry() const override;
    bool ignoresShield() const override;

    BestiaryInfo getBestiaryInfo() const override;
    void performDrops(Character* enemy, Character* currentPlayer, std::vector<std::string>& obtainedItems, int& totalGold, int& totalXp) override;

    // Compatibilidade legada
    void aoSofrerParryPerfeito() override { onSufferingPerfectParry(); }
    void aoTerAtaqueBloqueadoPorEscudo() { onAttackBlockedByShield(); }
    bool ignoraParry() const override { return ignoresParry(); }
    bool ignoraEscudo() const override { return ignoresShield(); }
};
