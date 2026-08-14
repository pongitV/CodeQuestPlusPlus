#pragma once

#include "../ClassBase.h"

class Item;
class Combat;

class Warrior : public ClassBase
{
public:
    // INFORMAÇÕES DA CLASSE
    std::string getClassName() const override; 
    ClassType getClassType() const override { return ClassType::Warrior; } 
    std::string getSpritePath() const override { return "assets/classes/knight.png"; }
    const std::vector<std::string>& getClassMenuAppearance() const override;
    Attributes getClassAttributes() const override;
    std::vector<std::unique_ptr<Item>> getClassEquipment() const override;

    // PASSIVA DA CLASSE
    std::string getClassPassiveName() const override;
    std::string getClassPassiveDescription() const override;

    // HABILIDADE DA CLASSE
    std::string getClassAbilityCooldownDescription() const override;
    std::string getClassAbilityName() const override;
    std::string getClassAbilityDescription() const override;
    void useClassAbility(Combat* combat, Character* userCharacter, std::vector<Character*>& enemyList) override;

protected:
    // PROCESSAMENTO DE DANO 
    int processPreAttackDamage(Character* attacker, Character* defender, int baseDamage, bool isAttackerPlayer, size_t enemyCount) override;
};  
