#pragma once

#include "../ClassBase.h"

class Item;
class Combat;

class Archer : public ClassBase 
{
public:
    // INFORMACOES DA CLASSE
    std::string getClassName() const override; 
    ClassType getClassType() const override { return ClassType::Archer; } 
    std::string getSpritePath() const override { return "assets/classes/archer.png"; }
    const std::vector<std::string>& getClassMenuAppearance() const override;
    Attributes getClassAttributes() const override;
    std::vector<std::unique_ptr<Item>> getClassEquipment() const override;

    // PASSIVA DA CLASSE
    std::string getClassPassiveName() const override;
    std::string getClassPassiveDescription() const override;
    int processArcherPassiveArmorPenalty(int basePenalty) const override;
    int applyArcherPassiveSlowPenalty(int currentDexterity) const override;
    int revertArcherPassiveSlowPenalty(int currentDexterity) const override;

    // HABILIDADE DA CLASSE
    std::string getClassAbilityCooldownDescription() const override;
    std::string getClassAbilityName() const override;
    std::string getClassAbilityDescription() const override;
    void useClassAbility(Combat* combat, Character* userCharacter, std::vector<Character*>& enemyList) override;
};
