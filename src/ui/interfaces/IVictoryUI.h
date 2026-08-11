#pragma once
#include <string>
#include <vector>
#include <utility>
#include "../../entities/character/Character.h"

class IVictoryUI {
public:
    virtual ~IVictoryUI() = default;
    virtual void display(Character* player, int goldEarned, int xpEarned, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns, const std::vector<std::string>& defeatedEnemies, int perfectParries, int maxDamage, int attemptedParries, int effectiveParries, int itemsConsumed, const std::vector<std::pair<std::string, int>>& uniqueDrops, bool canLevelUp, const std::vector<std::string>& newDiscoveries, const std::string& mapTitle) = 0;
};

using IVictoryUI = IVictoryUI;
