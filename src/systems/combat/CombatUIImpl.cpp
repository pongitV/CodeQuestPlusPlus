#include "CombatUIImpl.h"

#include "../../ui/screens/combat/ScreenCombat.h"
#include "../../ui/screens/victory/ScreenVictory.h"
#include "../../ui/screens/defeat/ScreenDefeat.h"
#include "../../ui/screens/attributes/ScreenAttributes.h"
#include "../../ui/screens/diary/ScreenDiary.h"

void CombatUIImpl::configure3DContext(bool is3D, const std::vector<std::string>& matrix, float posX, float posY, float angle, const std::string& title) {
    TelaCombate::configurarContexto3D(is3D, matrix, posX, posY, angle, title);
}

void CombatUIImpl::animateCombatIntroduction(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer) {
    TelaCombate::animarIntroducaoCombate(title, enemies, currentPlayer);
}

void CombatUIImpl::updateStaticScreen(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* currentPlayer, const std::vector<Character*>& allyList, bool animateEntry) {
    TelaCombate::atualizarTelaEstatica(combatTitle, enemyList, currentPlayer, allyList, animateEntry);
}

void CombatUIImpl::animateDamageOnEnemy(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* animationTarget, Character* attacker, Character* currentPlayer, const std::vector<Character*>& allyList, int animationDamage) {
    TelaCombate::animarDanoNoInimigo(combatTitle, enemyList, animationTarget, attacker, currentPlayer, allyList, animationDamage);
}

void CombatUIImpl::animateHealOnEnemy(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* animationTarget, Character* currentPlayer, const std::vector<Character*>& allyList, int animationHeal) {
    TelaCombate::animarCuraNoInimigo(combatTitle, enemyList, animationTarget, currentPlayer, allyList, animationHeal);
}

void CombatUIImpl::animateDamageOnPlayer(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* animationTarget, Character* currentPlayer, const std::vector<Character*>& allyList, bool isParry, int animationDamage) {
    TelaCombate::animarDanoNoJogador(combatTitle, enemyList, animationTarget, currentPlayer, allyList, isParry, animationDamage);
}

void CombatUIImpl::animateHealOnPlayer(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* animationTarget, Character* currentPlayer, const std::vector<Character*>& allyList, int animationHeal) {
    TelaCombate::animarCuraNoJogador(combatTitle, enemyList, animationTarget, currentPlayer, allyList, animationHeal);
}

void CombatUIImpl::animateEnemyDeath(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* deadEnemy, Character* currentPlayer, const std::vector<Character*>& allyList, const std::vector<std::string>& drops) {
    TelaCombate::animarMorteInimigo(combatTitle, enemyList, deadEnemy, currentPlayer, allyList, drops);
}

void CombatUIImpl::clearCharacterHUDContext() {
    TelaCombate::contexto.personagemHUD = nullptr;
}

void CombatUIImpl::clearDeadEnemyAndDropsContext() {
    TelaCombate::contexto.inimigoMortoComDrops = nullptr;
    TelaCombate::contexto.dropsAtivos.clear();
}

std::string CombatUIImpl::combatMargin() {
    return TelaCombate::margemCombate();
}

void CombatUIImpl::addFixedMessage(const std::string& msg) {
    TelaCombate::adicionarMensagemFixa(msg);
}

void CombatUIImpl::clearFixedMessages() {
    TelaCombate::limparMensagensFixas();
}

void CombatUIImpl::setVisibleTurn(int turn, const std::string& name) {
    TelaCombate::definirTurnoVisivel(turn, name);
}

int CombatUIImpl::getPlayerAction(int currentTurn, Character* actingCharacter, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return TelaCombate::obterAcaoDoJogador(currentTurn, actingCharacter, enemies, currentPlayer, allies);
}

int CombatUIImpl::getAttackTarget(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return TelaCombate::obterAlvoAtaque(combatTitle, enemies, currentPlayer, allies);
}

int CombatUIImpl::getItemTarget(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return TelaCombate::obterAlvoItem(combatTitle, enemies, currentPlayer, allies);
}

int CombatUIImpl::getShieldChoice(const std::string& characterName, const std::vector<Item*>& shieldList) {
    return TelaCombate::obterEscolhaDeEscudo(characterName, shieldList);
}

void CombatUIImpl::notifyEnemiesFaster() {
    TelaCombate::notificarInimigosMaisAgeis();
}

void CombatUIImpl::notifyExtraTurn(int playerDexterity, int maxEnemyDexterity) {
    TelaCombate::notificarTurnoExtra(playerDexterity, maxEnemyDexterity);
}

void CombatUIImpl::notifyInventoryUnready() {
    TelaCombate::notificarDesprevencaoInventario();
}

void CombatUIImpl::notifyNoShields(const std::string& characterName) {
    TelaCombate::notificarSemEscudos(characterName);
}

void CombatUIImpl::notifyDefenseImbalance(const std::string& characterName) {
    TelaCombate::notificarDesequilibrioDefesa(characterName);
}

void CombatUIImpl::notifyDefensiveStance(const std::string& characterName, const std::string& shieldName) {
    TelaCombate::notificarPosturaDefensiva(characterName, shieldName);
}

void CombatUIImpl::notifyInvalidAction() {
    TelaCombate::notificarAcaoInvalida();
}

void CombatUIImpl::notifyItemCancelled() {
    TelaCombate::notificarCancelamentoItem();
}

void CombatUIImpl::notifyRequirementNotMet(const std::string& requirementMessage) {
    TelaCombate::notificarRequisitoNaoAtendido(requirementMessage);
}

void CombatUIImpl::showVictoryScreen(Character* currentPlayer, int goldEarned, int xpEarned, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns, const std::vector<std::string>& obtainedItems, const std::vector<std::string>& defeatedEnemies, int perfectParries, int highestDamage, int attemptedParries, int effectiveParries, int consumedItems, const std::vector<std::string>& newDiscoveries) {
    TelaVitoria::display(currentPlayer, goldEarned, xpEarned, totalDamageDealt, totalDamageTaken, totalHealingReceived, combatTurns, obtainedItems, defeatedEnemies, perfectParries, highestDamage, attemptedParries, effectiveParries, consumedItems, newDiscoveries);
}

void CombatUIImpl::showDefeatScreen(Character* currentPlayer, int goldEarned, int xpEarned, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns) {
    TelaDerrota::display(currentPlayer, goldEarned, xpEarned, totalDamageDealt, totalDamageTaken, totalHealingReceived, combatTurns);
}

void CombatUIImpl::showAttributesScreen(Character* character) {
    TelaAtributos::gerenciarFichaDoJogador(character);
}

void CombatUIImpl::showDiaryScreen(Character* character) {
    TelaDiario::display(character);
}

void CombatUIImpl::clearScreen() {
}
