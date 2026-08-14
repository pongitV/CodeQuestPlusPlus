#include "EnemyMechanics.h"
#include "../../../entities/character/Character.h"
#include "../../../core/utils/RandomGenerator.h"

Character* EnemyMechanics::selectTarget(const std::vector<Character*>& livingAllies, Character* currentPlayer) {
    std::vector<Character*> candidateTargets;
    std::vector<Character*> livingMinions;
    std::vector<Character*> livingNormalAllies;

    for (auto* ally : livingAllies) {
        if (ally->isMinion()) {
            livingMinions.push_back(ally);
        } else {
            livingNormalAllies.push_back(ally);
        }
    }

    if (!livingMinions.empty()) {
        candidateTargets = livingMinions;
    } else if (!livingNormalAllies.empty()) {
        candidateTargets = livingNormalAllies;
    } else {
        candidateTargets.push_back(currentPlayer);
    }

    return candidateTargets[RandomGenerator::getInt(0, static_cast<int>(candidateTargets.size()) - 1)];
}
