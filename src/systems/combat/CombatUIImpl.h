#pragma once

#include "ICombatUI.h"

class CombatUIImpl : public ICombatUI {
public:
    CombatUIImpl() = default;
    ~CombatUIImpl() override = default;

    void configure3DContext(bool is3D, const std::vector<std::string>& matrix, float posX, float posY, float angle, const std::string& title) override;
    
    void animateCombatIntroduction(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer) override;
    void updateStaticScreen(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* currentPlayer, const std::vector<Character*>& allyList, bool animateEntry = false) override;
    
    void animateDamageOnEnemy(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* animationTarget, Character* attacker, Character* currentPlayer, const std::vector<Character*>& allyList, int animationDamage) override;
    void animateHealOnEnemy(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* animationTarget, Character* currentPlayer, const std::vector<Character*>& allyList, int animationHeal) override;
    
    void animateDamageOnPlayer(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* animationTarget, Character* currentPlayer, const std::vector<Character*>& allyList, bool isParry, int animationDamage) override;
    void animateHealOnPlayer(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* animationTarget, Character* currentPlayer, const std::vector<Character*>& allyList, int animationHeal) override;
    
    void animateEnemyDeath(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* deadEnemy, Character* currentPlayer, const std::vector<Character*>& allyList, const std::vector<std::string>& drops) override;

    void clearCharacterHUDContext() override;
    void clearDeadEnemyAndDropsContext() override;

    std::string combatMargin() override;

    void addFixedMessage(const std::string& msg) override;
    void clearFixedMessages() override;
    void setBannerMessage(const std::string& msg, CorBanner color = CorBanner::OURO, const std::string& msgLine2 = "") override {}
    void setVisibleTurn(int turn, const std::string& name) override;
    
    int getPlayerAction(int currentTurn, Character* actingCharacter, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) override;
    int getAttackTarget(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) override;
    int getItemTarget(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) override;
    int getShieldChoice(const std::string& characterName, const std::vector<Item*>& shieldList) override;
    
    void notifyEnemiesFaster() override;
    void notifyExtraTurn(int playerDexterity, int maxEnemyDexterity) override;
    void notifyInventoryUnready() override;
    void notifyNoShields(const std::string& characterName) override;
    void notifyDefenseImbalance(const std::string& characterName) override;
    void notifyDefensiveStance(const std::string& characterName, const std::string& shieldName) override;
    void notifyInvalidAction() override;
    void notifyItemCancelled() override;
    void notifyRequirementNotMet(const std::string& requirementMessage) override;

    void showVictoryScreen(Character* currentPlayer, int goldEarned, int xpEarned, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns, const std::vector<std::string>& obtainedItems, const std::vector<std::string>& defeatedEnemies, int perfectParries, int highestDamage, int attemptedParries, int effectiveParries, int consumedItems, const std::vector<std::string>& newDiscoveries) override;
    void showDefeatScreen(Character* currentPlayer, int goldEarned, int xpEarned, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns) override;

    void showAttributesScreen(Character* character) override;
    void showDiaryScreen(Character* character) override;

    void clearScreen() override;
};

using CombateUIImpl = CombatUIImpl;
