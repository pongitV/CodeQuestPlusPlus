#pragma once

#include "../ClassBase.h"
#include "../../races/RaceBase.h"
#include <string>
#include <vector>
#include <memory>

class Item;
class Combat;
class Character;

class RaceClone : public RaceBase {
private:
    std::string originalName;
    std::vector<std::string> originalAppearance;
public:
    RaceClone(const std::string& name, const std::vector<std::string>& appearance);
    std::string getRaceName() const override;
    RaceType getRaceType() const override;
    const std::vector<std::string>& getRaceAppearance() const override;
    Attributes getRaceAttributes() const override;
    std::string getRaceAbilityName() const override;
    std::string getRaceAbilityDescription() const override;
};

class PlayerClassClone : public ClassBase {
public:
    std::string getClassName() const override;
    ClassType getClassType() const override;
    const std::vector<std::string>& getClassMenuAppearance() const override;
    Attributes getClassAttributes() const override;
    std::vector<std::unique_ptr<Item>> getClassEquipment() const override;
    std::string getClassPassiveName() const override;
    std::string getClassPassiveDescription() const override;
    std::string getClassAbilityCooldownDescription() const override;
    std::string getClassAbilityName() const override;
    std::string getClassAbilityDescription() const override;
    void useClassAbility(Combat*, Character*, std::vector<Character*>&) override;
};
