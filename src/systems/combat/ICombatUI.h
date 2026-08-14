#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../../rendering/raycaster/engine-raycaster/RaycasterHUD.h"

class Character;
class Item;

class ICombatUI {
public:
    virtual ~ICombatUI() = default;

    virtual void configure3DContext(bool is3D, const std::vector<std::string>& matrix, float posX, float posY, float angle, const std::string& title) = 0;
    
    virtual void animateCombatIntroduction(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer) = 0;
    virtual void updateStaticScreen(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* currentPlayer, const std::vector<Character*>& allyList, bool animateEntry = false) = 0;
    
    virtual void animateDamageOnEnemy(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* animationTarget, Character* attacker, Character* currentPlayer, const std::vector<Character*>& allyList, int animationDamage) = 0;
    virtual void animateHealOnEnemy(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* animationTarget, Character* currentPlayer, const std::vector<Character*>& allyList, int animationHeal) = 0;
    
    virtual void animateDamageOnPlayer(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* animationTarget, Character* currentPlayer, const std::vector<Character*>& allyList, bool isParry, int animationDamage) = 0;
    virtual void animateHealOnPlayer(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* animationTarget, Character* currentPlayer, const std::vector<Character*>& allyList, int animationHeal) = 0;
    
    virtual void animateEnemyDeath(const std::string& combatTitle, const std::vector<Character*>& enemyList, Character* deadEnemy, Character* currentPlayer, const std::vector<Character*>& allyList, const std::vector<std::string>& drops) = 0;

    virtual void clearCharacterHUDContext() = 0;
    virtual void clearDeadEnemyAndDropsContext() = 0;

    virtual std::string combatMargin() = 0;

    virtual void addFixedMessage(const std::string& msg) = 0;
    virtual void clearFixedMessages() = 0;
    virtual void setBannerMessage(const std::string& msg, CorBanner color = CorBanner::OURO, const std::string& msgLine2 = "") = 0;
    virtual void setVisibleTurn(int turn, const std::string& name) = 0;
    
    virtual int getPlayerAction(int currentTurn, Character* actingCharacter, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) = 0;
    virtual int getAttackTarget(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) = 0;
    virtual int getItemTarget(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) = 0;
    virtual int getShieldChoice(const std::string& characterName, const std::vector<Item*>& shieldList) = 0;
    
    virtual void notifyEnemiesFaster() = 0;
    virtual void notifyExtraTurn(int playerDexterity, int maxEnemyDexterity) = 0;
    virtual void notifyInventoryUnready() = 0;
    virtual void notifyNoShields(const std::string& characterName) = 0;
    virtual void notifyDefenseImbalance(const std::string& characterName) = 0;
    virtual void notifyDefensiveStance(const std::string& characterName, const std::string& shieldName) = 0;
    virtual void notifyInvalidAction() = 0;
    virtual void notifyItemCancelled() = 0;
    virtual void notifyRequirementNotMet(const std::string& requirementMessage) = 0;

    virtual void showVictoryScreen(Character* currentPlayer, int goldEarned, int xpEarned, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns, const std::vector<std::string>& obtainedItems, const std::vector<std::string>& defeatedEnemies, int perfectParries, int highestDamage, int attemptedParries, int effectiveParries, int consumedItems, const std::vector<std::string>& newDiscoveries) = 0;
    virtual void showDefeatScreen(Character* currentPlayer, int goldEarned, int xpEarned, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns) = 0;
    
    virtual void showAttributesScreen(Character* character) = 0;
    virtual void showDiaryScreen(Character* character) = 0;

    virtual void clearScreen() = 0;

    // Metodos legados com implementacao padrao para retrocompatibilidade
    virtual void configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) {
        configure3DContext(modo3D, matriz, posX, posY, angulo, titulo);
    }
    virtual void animarIntroducaoCombate(const std::string& titulo, const std::vector<Character*>& enemies, Character* currentPlayer) {
        animateCombatIntroduction(titulo, enemies, currentPlayer);
    }
    virtual void atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, bool animarEntrada = false) {
        updateStaticScreen(tituloCombate, listaDeInimigos, currentPlayer, listaDeAliados, animarEntrada);
    }
    virtual void animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* atacante, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int danoAnimacao) {
        animateDamageOnEnemy(tituloCombate, listaDeInimigos, alvoAnimacao, atacante, currentPlayer, listaDeAliados, danoAnimacao);
    }
    virtual void animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int curaAnimacao) {
        animateHealOnEnemy(tituloCombate, listaDeInimigos, alvoAnimacao, currentPlayer, listaDeAliados, curaAnimacao);
    }
    virtual void animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, bool isParry, int danoAnimacao) {
        animateDamageOnPlayer(tituloCombate, listaDeInimigos, alvoAnimacao, currentPlayer, listaDeAliados, isParry, danoAnimacao);
    }
    virtual void animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int curaAnimacao) {
        animateHealOnPlayer(tituloCombate, listaDeInimigos, alvoAnimacao, currentPlayer, listaDeAliados, curaAnimacao);
    }
    virtual void animarMorteInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* inimigoMorto, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, const std::vector<std::string>& drops) {
        animateEnemyDeath(tituloCombate, listaDeInimigos, inimigoMorto, currentPlayer, listaDeAliados, drops);
    }
    virtual void limparContextoPersonagemHUD() { clearCharacterHUDContext(); }
    virtual void limparContextoInimigoMortoEDrops() { clearDeadEnemyAndDropsContext(); }
    virtual std::string margemCombate() { return combatMargin(); }
    virtual void adicionarMensagemFixa(const std::string& msg) { addFixedMessage(msg); }
    virtual void limparMensagensFixas() { clearFixedMessages(); }
    virtual void definirMensagemBanner(const std::string& msg, CorBanner cor = CorBanner::OURO, const std::string& msgLinha2 = "") {
        setBannerMessage(msg, cor, msgLinha2);
    }
    virtual void definirTurnoVisivel(int turno, const std::string& nome) { setVisibleTurn(turno, nome); }
    virtual int obterAcaoDoJogador(int turnoAtual, Character* personagemAgindo, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) {
        return getPlayerAction(turnoAtual, personagemAgindo, enemies, currentPlayer, aliados);
    }
    virtual int obterAlvoAtaque(const std::string& tituloCombate, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) {
        return getAttackTarget(tituloCombate, enemies, currentPlayer, aliados);
    }
    virtual int obterAlvoItem(const std::string& tituloCombate, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) {
        return getItemTarget(tituloCombate, enemies, currentPlayer, aliados);
    }
    virtual int obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos) {
        return getShieldChoice(nomePersonagem, listaDeEscudos);
    }
    virtual void notificarInimigosMaisAgeis() { notifyEnemiesFaster(); }
    virtual void notificarTurnoExtra(int dexterityJogador, int maxDestrezaInimigos) { notifyExtraTurn(dexterityJogador, maxDestrezaInimigos); }
    virtual void notificarDesprevencaoInventario() { notifyInventoryUnready(); }
    virtual void notificarSemEscudos(const std::string& nomePersonagem) { notifyNoShields(nomePersonagem); }
    virtual void notificarDesequilibrioDefesa(const std::string& nomePersonagem) { notifyDefenseImbalance(nomePersonagem); }
    virtual void notificarPosturaDefensiva(const std::string& nomePersonagem, const std::string& nomeEscudo) { notifyDefensiveStance(nomePersonagem, nomeEscudo); }
    virtual void notificarAcaoInvalida() { notifyInvalidAction(); }
    virtual void notificarCancelamentoItem() { notifyItemCancelled(); }
    virtual void notificarRequisitoNaoAtendido(const std::string& mensagemRequisito) { notifyRequirementNotMet(mensagemRequisito); }
    virtual void displayTelaVitoria(Character* currentPlayer, int amountDeOuroObtido, int amountDeXpObtido, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns, const std::vector<std::string>& itensObtidos, const std::vector<std::string>& inimigosDerrotados, int parriesPerfeitos, int maiorDano, int parriesTentados, int parriesEfetivos, int itensConsumidos, const std::vector<std::string>& novasDescobertas) {
        showVictoryScreen(currentPlayer, amountDeOuroObtido, amountDeXpObtido, totalDamageDealt, totalDamageTaken, totalHealingReceived, combatTurns, itensObtidos, inimigosDerrotados, parriesPerfeitos, maiorDano, parriesTentados, parriesEfetivos, itensConsumidos, novasDescobertas);
    }
    virtual void displayTelaDerrota(Character* currentPlayer, int amountDeOuroObtido, int amountDeXpObtido, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns) {
        showDefeatScreen(currentPlayer, amountDeOuroObtido, amountDeXpObtido, totalDamageDealt, totalDamageTaken, totalHealingReceived, combatTurns);
    }
    virtual void displayTelaAtributos(Character* character) { showAttributesScreen(character); }
    virtual void displayTelaDiario(Character* character) { showDiaryScreen(character); }
    virtual void limparTela() { clearScreen(); }
};

using ICombateUI = ICombatUI;
