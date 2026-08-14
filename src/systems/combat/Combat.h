#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <chrono>
#include <unordered_map>

#include "ICombatUI.h"
#include "../../entities/character/Character.h"

/**
 * @brief Classe responsavel por gerenciar o fluxo de combate do jogo.
 * Controla turnos, vida, acoes e UI do combate.
 */
class Combat 
{
public:
    enum class CombatAction 
    { 
        Attack = 1, 
        Defend = 2, 
        Ability = 3, 
        Inventory = 4, 
        Player = 5, 
        Bestiary = 6,

        // Aliases legados
        Atacar = Attack,
        Defender = Defend,
        Habilidade = Ability,
        Jogador = Player
    };

    using AcaoCombate = CombatAction;

private:
    // Referencias aos participantes do combate ativo
    Character* currentPlayer;
    std::vector<std::unique_ptr<Character>> enemyList;
    std::vector<std::unique_ptr<Character>> allyList;

    // Interface visual de combate (Injecao de Dependencia)
    std::unique_ptr<ICombatUI> ui;

    // Constantes de Controle
    static constexpr int MAX_DEBUG_DAMAGE = 99999;
    static constexpr int NULL_DAMAGE = 0;

    // Estatisticas gerais e controle da sessao de combate
    int goldEarned;
    int xpEarned;
    int totalDamageDealt;
    int totalDamageTaken;
    int currentTurnCounter;
    std::vector<std::string> obtainedItems;
    std::vector<std::string> defeatedEnemies;

    // Estatisticas Avancadas da Sessao
    int stats_attemptedParries;
    int stats_effectiveParries;
    int stats_perfectParries;
    int stats_highestDamageDealt;
    int stats_consumedItems;
    std::vector<std::string> stats_newDiscoveries;
    void resetAdvancedStatistics();

    std::string getFormattedNameWithNumber(Character* c) const;
    void applyDamageToTarget(Character* attackingCharacter, Character* targetCharacter, int rawDamage, int piercingDamage, int currentCombatTurn);
    void processEnemyDeath(Character* enemy);
    void displayAttackResult(Character* attacker, Character* target, int finalDamage, bool attemptedParry, bool parrySuccess, int blockedDamage, bool shieldBroke, const std::string& brokenShieldName);

    void prepareCharacterTurn(Character* character);
    void processPostDamage(Character* attacker, Character* target, int finalDamage, bool attemptedParry, bool parrySuccess);
    bool isPlayerOrAlly(Character* character) const;
    void processPlayerActionMenu(Character* actingCharacter, bool& turnConsumed, bool& usedInventoryInTurn);
    void processAttackAction(Character* actingCharacter, bool& turnConsumed);
    void processDefendAction(Character* actingCharacter, bool& turnConsumed);
    void processAbilityAction(Character* actingCharacter, bool& turnConsumed);
    void processInventoryAction(Character* actingCharacter, bool& turnConsumed, bool& usedInventoryInTurn);
    void clearDeadEnemies();
    Item* selectShield(Character* actingCharacter);

    std::string getCombatTitle() const;
    std::vector<Character*> getRawEnemies() const;
    void displayCombatScreen(bool animateEntry = false) const;

public:
    /**
     * @brief Construtor que move ownership dos inimigos
     */
    Combat(Character* combatPlayer,
           std::vector<std::unique_ptr<Character>>&& combatEnemies,
           std::unique_ptr<ICombatUI> visualInterface = nullptr);
    virtual ~Combat();

    void set3DContext(bool is3D, const std::vector<std::string>& matrix, float posX, float posY, float angle, const std::string& title);

    std::vector<Character*> getLivingAlliesRaw() const;
    bool executePlayerOrAllyTurn(Character* character, bool& firstRender, bool processStartEffects = true);
    void addAllyInCombat(std::unique_ptr<Character> ally);
    void addLivingAllies(std::vector<std::unique_ptr<Character>> allies);
    void addCombatAllies(std::vector<std::unique_ptr<Character>> allies) { addLivingAllies(std::move(allies)); }
    
    void startCombat();
    void executeAllEnemiesTurn();
    bool checkWinLossCondition();
    void performPhysicalAttack(Character* attackingCharacter, Character* defendingCharacter, int currentCombatTurn);

    int getAttemptedParries() const { return stats_attemptedParries; }
    int getEffectiveParries() const { return stats_effectiveParries; }
    int getHighestDamageDealt() const { return stats_highestDamageDealt; }
    int getConsumedItems() const { return stats_consumedItems; }
    const std::vector<std::string>& getNewDiscoveries() const { return stats_newDiscoveries; }

    // Compatibilidade legada
    void setContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) {
        set3DContext(modo3D, matriz, posX, posY, angulo, titulo);
    }
    std::vector<Character*> obterAliadosVivosRaw() const { return getLivingAlliesRaw(); }
    bool executarTurnoJogadorOuAliado(Character* character, bool& primeiraRenderizacao, bool processarEfeitosInicio = true) {
        return executePlayerOrAllyTurn(character, primeiraRenderizacao, processarEfeitosInicio);
    }
    void adicionarAliadoEmCombate(std::unique_ptr<Character> aliado) { addAllyInCombat(std::move(aliado)); }
    void adicionarAliados(std::vector<std::unique_ptr<Character>> aliados) { addLivingAllies(std::move(aliados)); }
    void iniciarCombate() { startCombat(); }
    void executarTurnoDeTodosOsInimigos() { executeAllEnemiesTurn(); }
    bool verificarCondicaoDeVitoriaOuDerrota() { return checkWinLossCondition(); }
    void realizarAtaqueFisico(Character* atacante, Character* defensor, int turno) { performPhysicalAttack(atacante, defensor, turno); }

    int obterParriesTentados() const { return getAttemptedParries(); }
    int obterParriesEfetivos() const { return getEffectiveParries(); }
    int obterMaiorDanoCausado() const { return getHighestDamageDealt(); }
    int obterItensConsumidos() const { return getConsumedItems(); }
    const std::vector<std::string>& obterNovasDescobertas() const { return getNewDiscoveries(); }
};

struct CombatUIBatch {
    std::vector<std::string> messages;
    std::vector<std::pair<Character*, int>> animatedDamages;
    
    void clear() {
        messages.clear();
        animatedDamages.clear();
    }
    void limpar() { clear(); }
};

struct CombatState {
    int currentTurn = 0;
    std::vector<Character*> actionOrder;
    bool combatChanged = true;
};

struct CombatPrediction {
    std::vector<std::pair<Character*, int>> predictedAttacks;
    std::chrono::steady_clock::time_point predictionTimestamp;
    static constexpr auto PREDICTION_DURATION = std::chrono::milliseconds(500);

    bool needsRecalculation() const {
        return (std::chrono::steady_clock::now() - predictionTimestamp) > PREDICTION_DURATION;
    }
    bool precisaRecalcular() const { return needsRecalculation(); }
};

struct DamageCache {
    std::unordered_map<Character*, int> baseDamage;
    std::unordered_map<Character*, int> defenseReduction;
    bool dirty = true;

    void invalidate() { dirty = true; }
    void invalidar() { invalidate(); }
};
