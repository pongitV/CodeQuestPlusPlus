#include "CombatRaycasterUIImpl.h"

#include "../../rendering/raycaster/screens/combat/ScreenCombatRaycaster.h"
#include "../../rendering/raycaster/screens/victory/ScreenVictoryRaycaster.h"
#include "../../rendering/raycaster/screens/defeat/ScreenDefeatRaycaster.h"
#include "../../ui/screens/attributes/ScreenAttributes.h"
#include "../../ui/screens/diary/ScreenDiary.h"
#include "../../ui/screens/combat/ScreenCombat.h"
#include "Parry.h"

void CombatRaycasterUIImpl::configure3DContext(bool is3D, const std::vector<std::string>& matrix, float posX, float posY, float angle, const std::string& title) {
    TelaCombateRaycaster::configurarContexto3D(is3D, matrix, posX, posY, angle, title);
}

void CombatRaycasterUIImpl::animateCombatIntroduction(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer) {
    TelaCombateRaycaster::animarIntroducaoCombate(title, enemies, currentPlayer);
}

void CombatRaycasterUIImpl::updateStaticScreen(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* currentPlayer, const std::vector<Character*>& allyList, bool animateEntry) {
    Parry::onUpdateScreen = [=]() {
        TelaCombateRaycaster::atualizarTelaEstatica(combatTitle, enemyList, currentPlayer, allyList, false, nullptr);
    };
    TelaCombateRaycaster::atualizarTelaEstatica(combatTitle, enemyList, currentPlayer, allyList, animateEntry, nullptr);
}

void CombatRaycasterUIImpl::animateDamageOnEnemy(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* animationTarget, Character* attacker, Character* currentPlayer, const std::vector<Character*>& allyList, int animationDamage) {
    TelaCombateRaycaster::animarDanoNoInimigo(combatTitle, enemyList, animationTarget, attacker, currentPlayer, allyList, animationDamage);
}

void CombatRaycasterUIImpl::animateHealOnEnemy(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* animationTarget, Character* currentPlayer, const std::vector<Character*>& allyList, int animationHeal) {
    TelaCombateRaycaster::animarCuraNoInimigo(combatTitle, enemyList, animationTarget, currentPlayer, allyList, animationHeal);
}

void CombatRaycasterUIImpl::animateDamageOnPlayer(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* animationTarget, Character* currentPlayer, const std::vector<Character*>& allyList, bool isParry, int animationDamage) {
    TelaCombateRaycaster::animarDanoNoJogador(combatTitle, enemyList, animationTarget, currentPlayer, allyList, isParry, animationDamage);
}

void CombatRaycasterUIImpl::animateHealOnPlayer(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* animationTarget, Character* currentPlayer, const std::vector<Character*>& allyList, int animationHeal) {
    TelaCombateRaycaster::animarCuraNoJogador(combatTitle, enemyList, animationTarget, currentPlayer, allyList, animationHeal);
}

void CombatRaycasterUIImpl::animateEnemyDeath(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* deadEnemy, Character* currentPlayer, const std::vector<Character*>& allyList, const std::vector<std::string>& drops) {
    TelaCombateRaycaster::animarMorteInimigo(combatTitle, enemyList, deadEnemy, currentPlayer, allyList, drops);
}

void CombatRaycasterUIImpl::clearCharacterHUDContext() {
    TelaCombate::contexto.personagemHUD = nullptr;
}

void CombatRaycasterUIImpl::clearDeadEnemyAndDropsContext() {
    TelaCombate::contexto.inimigoMortoComDrops = nullptr;
    TelaCombate::contexto.dropsAtivos.clear();
}

std::string CombatRaycasterUIImpl::combatMargin() {
    return ""; // TelaCombateRaycaster usa formatacao propria para mensagens fixas em 3D
}

void CombatRaycasterUIImpl::addFixedMessage(const std::string& msg) {
    TelaCombateRaycaster::adicionarMensagemFixa(msg);
}

void CombatRaycasterUIImpl::clearFixedMessages() {
    TelaCombateRaycaster::limparMensagensFixas();
}

void CombatRaycasterUIImpl::setBannerMessage(const std::string& msg, CorBanner color, const std::string& msgLine2) {
    TelaCombateRaycaster::definirMensagemBanner(msg, color, msgLine2);
}

void CombatRaycasterUIImpl::setVisibleTurn(int turn, const std::string& name) {
    TelaCombateRaycaster::definirTurnoVisivel(turn, name);
}

int CombatRaycasterUIImpl::getPlayerAction(int currentTurn, Character* actingCharacter, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return TelaCombateRaycaster::obterAcaoDoJogador(currentTurn, actingCharacter, enemies, currentPlayer, allies);
}

int CombatRaycasterUIImpl::getAttackTarget(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return TelaCombateRaycaster::obterAlvoAtaque(combatTitle, enemies, currentPlayer, allies);
}

int CombatRaycasterUIImpl::getItemTarget(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return TelaCombateRaycaster::obterAlvoItem(combatTitle, enemies, currentPlayer, allies);
}

int CombatRaycasterUIImpl::getShieldChoice(const std::string& characterName, const std::vector<Item*>& shieldList) {
    return TelaCombateRaycaster::obterEscolhaDeEscudo(characterName, shieldList);
}

void CombatRaycasterUIImpl::notifyEnemiesFaster() {
    TelaCombateRaycaster::notificarInimigosMaisAgeis();
}

void CombatRaycasterUIImpl::notifyExtraTurn(int playerDexterity, int maxEnemyDexterity) {
    TelaCombateRaycaster::notificarTurnoExtra(playerDexterity, maxEnemyDexterity);
}

void CombatRaycasterUIImpl::notifyInventoryUnready() {
    TelaCombateRaycaster::notificarDesprevencaoInventario();
}

void CombatRaycasterUIImpl::notifyNoShields(const std::string& characterName) {
    TelaCombateRaycaster::notificarSemEscudos(characterName);
}

void CombatRaycasterUIImpl::notifyDefenseImbalance(const std::string& characterName) {
    TelaCombateRaycaster::notificarDesequilibrioDefesa(characterName);
}

void CombatRaycasterUIImpl::notifyDefensiveStance(const std::string& characterName, const std::string& shieldName) {
    TelaCombateRaycaster::notificarPosturaDefensiva(characterName, shieldName);
}

void CombatRaycasterUIImpl::notifyInvalidAction() {
    TelaCombateRaycaster::notificarAcaoInvalida();
}

void CombatRaycasterUIImpl::notifyItemCancelled() {
    TelaCombateRaycaster::notificarCancelamentoItem();
}

void CombatRaycasterUIImpl::notifyRequirementNotMet(const std::string& requirementMessage) {
    TelaCombateRaycaster::notificarRequisitoNaoAtendido(requirementMessage);
}

void CombatRaycasterUIImpl::showVictoryScreen(Character* currentPlayer, int goldEarned, int xpEarned, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns, const std::vector<std::string>& obtainedItems, const std::vector<std::string>& defeatedEnemies, int perfectParries, int highestDamage, int attemptedParries, int effectiveParries, int consumedItems, const std::vector<std::string>& newDiscoveries) {
    std::unordered_map<std::string, int> dropsFrequency;
    for (const auto& item : obtainedItems) {
        dropsFrequency[item]++;
    }
    std::vector<std::pair<std::string, int>> uniqueDrops;
    for (const auto& pair : dropsFrequency) {
        uniqueDrops.push_back(pair);
    }

    bool canLevelUp = currentPlayer->getCurrentXp() + xpEarned >= currentPlayer->getRequiredXpForLevelUp();

    TelaVitoriaRaycaster::display(currentPlayer, goldEarned, xpEarned, totalDamageDealt, totalDamageTaken, totalHealingReceived, combatTurns, defeatedEnemies, perfectParries, highestDamage, attemptedParries, effectiveParries, consumedItems, uniqueDrops, canLevelUp, newDiscoveries, "");
}

void CombatRaycasterUIImpl::showDefeatScreen(Character* currentPlayer, int goldEarned, int xpEarned, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns) {
    TelaDerrotaRaycaster::display(currentPlayer, goldEarned, xpEarned, totalDamageDealt, totalDamageTaken, totalHealingReceived, combatTurns);
}

void CombatRaycasterUIImpl::showAttributesScreen(Character* character) {
    TelaAtributos::gerenciarFichaDoJogador(character);
}

void CombatRaycasterUIImpl::showDiaryScreen(Character* character) {
    TelaDiario::display(character);
}

void CombatRaycasterUIImpl::clearScreen() {
}
