#include "ClassBaseEnemy.h"

// --- INFORMACOES DA CLASSE ---
std::string ClassBaseEnemy::getClassName() const 
{ 
    return "Monstro"; 
}

Attributes ClassBaseEnemy::getClassAttributes() const 
{ 
    return { 0, 0, 0, 0, 0, 0, 0 }; 
}

const std::vector<std::string>& ClassBaseEnemy::getClassMenuAppearance() const 
{ 
    static const std::vector<std::string> emptyAppearance = {};
    return emptyAppearance;
}

std::vector<std::unique_ptr<Item>> ClassBaseEnemy::getClassEquipment() const 
{ 
    return {}; 
}

// --- PASSIVA DA CLASSE ---
std::string ClassBaseEnemy::getClassPassiveName() const { return "Nenhuma"; }
std::string ClassBaseEnemy::getClassPassiveDescription() const { return "Enemies nao possuem passivas de classe."; }

// --- HABILIDADE DA CLASSE ---
std::string ClassBaseEnemy::getClassAbilityCooldownDescription() const { return ""; }
std::string ClassBaseEnemy::getClassAbilityName() const { return "Nenhuma"; }
std::string ClassBaseEnemy::getClassAbilityDescription() const { return "Enemies basicos nao possuem habilidades ativas."; }
void ClassBaseEnemy::useClassAbility(Combat* /*combat*/, Character* /*userCharacter*/, std::vector<Character*>& /*enemyList*/) 
{
}

AttackType ClassBaseEnemy::getAttackType() const { return AttackType::Single; }
bool ClassBaseEnemy::abilityConsumesTurn() const { return true; }
