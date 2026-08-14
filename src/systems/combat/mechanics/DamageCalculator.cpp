#include "DamageCalculator.h"
#include "../../../entities/interfaces/IAttacker.h"
#include "../../../entities/interfaces/IDamageable.h"

std::pair<int, int> DamageCalculator::calculateOffensiveBaseDamage(IAttacker* attacker) {
    if (attacker) {
        return attacker->calculateBaseOffensiveDamage();
    }
    return {0, 0};
}

int DamageCalculator::calculateDefensiveMitigation(IDamageable* target, int rawDamage, int piercingDamage) {
    if (target) {
        return target->calculateBaseDefense(rawDamage, piercingDamage);
    }
    return rawDamage + piercingDamage;
}
