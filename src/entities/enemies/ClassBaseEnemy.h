#pragma once

#include <string>
#include <vector>

#include "../classes/ClassBase.h"

class ClassBaseEnemy : public ClassBase
{
public:
    std::string getClassName() const override;
    ClassType getClassType() const override { return ClassType::None; }
    Attributes getClassAttributes() const override;
    const std::vector<std::string>& getClassMenuAppearance() const override;
    std::vector<std::unique_ptr<Item>> getClassEquipment() const override;

    std::string getClassPassiveName() const override;
    std::string getClassPassiveDescription() const override;
    std::string getClassAbilityCooldownDescription() const override;

    void useClassAbility(Combat* combat, Character* userCharacter, std::vector<Character*>& enemyList) override;
    std::string getClassAbilityName() const override;
    std::string getClassAbilityDescription() const override;
    
    AttackType getAttackType() const override;
    bool abilityConsumesTurn() const override;
};

using ClassBaseInimigo = ClassBaseEnemy;
