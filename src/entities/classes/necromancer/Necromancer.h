#pragma once

#include "../ClassBase.h"

class Necromancer : public ClassBase {
public:
    // Informacoes da classe
    std::string getClassName() const override;
    ClassType getClassType() const override { return ClassType::Necromancer; }
    std::string getSpritePath() const override { return "assets/classes/necromancer.png"; }
    const std::vector<std::string>& getClassMenuAppearance() const override;
    Attributes getClassAttributes() const override;
    std::vector<std::unique_ptr<Item>> getClassEquipment() const override;

    // Habilidade da classe
    std::string getClassAbilityName() const override;
    std::string getClassAbilityDescription() const override;
    std::string getClassAbilityCooldownDescription() const override;
    void useClassAbility(Combat* combat, Character* userCharacter, std::vector<Character*>& enemyList) override;

    // Passiva da classe
    std::string getClassPassiveName() const override;
    std::string getClassPassiveDescription() const override;
    void executeAttackWithClassPassive(Character* attacker, Character* defender, int baseDamage, int piercingDamage, std::vector<std::unique_ptr<Character>>& enemyList, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool applyPassive) override;
};
