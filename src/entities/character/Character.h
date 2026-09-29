#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>
#include <map>

#include "../common/EquipmentSlot.h"
#include "../interfaces/IAttacker.h"
#include "../interfaces/IDamageable.h"

#include "../../core/state/Status.h"
#include "../../systems/inventory/Inventory.h"
#include "../common/LevelSystem.h"
#include "../../core/utils/Color.h"

struct Attributes 
{
    int health;         // Pontos de vida maximos (HP) do personagem
    int strength;       // Influencia dano de ataques fisicos frontais e armas pesadas
    int dexterity;      // Determina ordem de turno, dano de armas ageis e acerto critico/esquiva
    int resistance;     // Reduz dano fisico recebido e age como requisito para escudos pesados
    int constitution;   // Vitalidade geral, reduz efetividade de debuffs e age como requisito para armadura
    int intelligence;   // Multiplicador base de dano magico e requisito para cajados/varinhas
    int wisdom;         // Aumenta atributos magicos secundarios, forca de curas e defesa magica

    void addAttributes(const Attributes& other) 
    {
        this->health += other.health;
        this->strength += other.strength;
        this->dexterity += other.dexterity;
        this->resistance += other.resistance;
        this->constitution += other.constitution;
        this->intelligence += other.intelligence;
        this->wisdom += other.wisdom;
    }

    void adjustAll(int delta) {
        this->strength = std::max(1, this->strength + delta);
        this->dexterity = std::max(1, this->dexterity + delta);
        this->resistance = std::max(1, this->resistance + delta);
        this->constitution = std::max(1, this->constitution + delta);
        this->intelligence = std::max(1, this->intelligence + delta);
        this->wisdom = std::max(1, this->wisdom + delta);
    }

    void scaleAll(double factor) {
        this->health = std::max(1, static_cast<int>(this->health * factor));
        this->strength = static_cast<int>(this->strength * factor);
        this->dexterity = static_cast<int>(this->dexterity * factor);
        this->resistance = static_cast<int>(this->resistance * factor);
        this->constitution = static_cast<int>(this->constitution * factor);
        this->intelligence = static_cast<int>(this->intelligence * factor);
        this->wisdom = static_cast<int>(this->wisdom * factor);
    }
};

enum class AttributeType 
{
    Health = 1,
    Strength,
    Dexterity,
    Resistance,
    Constitution,
    Intelligence,
    Wisdom
};

using TipoAtributo = AttributeType;

enum class AttackType 
{
    Single,
    Area
};

using TipoAtaque = AttackType;

class RaceBase;   
class ClassBase; 
enum class ClassType;
enum class AbilityID;
enum class RaceType;

enum class GameDifficulty 
{
    Easy = 1,
    Normal = 2,
    Hard = 3
};

using DificuldadeJogo = GameDifficulty;

// Classe central do jogo que representa qualquer entidade viva (Jogador, Inimigos, NPCs).
// Agrega status, atributos, inventario e logica de persistencia e interacao.
class Character : public IAttacker, public IDamageable
{
private:
    struct CombatControl {
        bool isDefending = false;
        std::vector<std::unique_ptr<Character>> collectedSouls;
        bool defenseCooldown = false;
        bool abilityCooldown = false;
        bool skipEnemyTurn = false;
        bool abilityCanceled = false;
        bool animatedDeath = false;
        double currentMultiplier = 1.0;
        int totalHealingReceived = 0;
        int fixedMaxHealth = 0;
        std::unordered_map<AbilityID, int> activeCooldowns;
        
        void reset() {
            isDefending = false;
            defenseCooldown = false;
            abilityCooldown = false;
            skipEnemyTurn = false;
            abilityCanceled = false;
            animatedDeath = false;
            currentMultiplier = 1.0;
            totalHealingReceived = 0;
            fixedMaxHealth = 0;
            activeCooldowns.clear();
        }
    };

    struct SystemControl {
        bool wantsToReturnToMenu = false;
        bool labyrinthUnlocked = false;
        bool canRevive = true;
        bool parryEnabled = true;
        bool modernParry = true;
        bool hasTrollRegeneration = false;
        bool godModeActive = false;
        bool noclipActive = false;
        bool isMinion = false;
        GameDifficulty currentDifficulty = GameDifficulty::Normal;
        double difficultyMultiplier = 1.0;
        char playerIcon = '@';
        Color playerColor = Color::GREEN;
        Color terminalBackgroundColor = Color::RESET;
    };

    static std::unordered_set<Character*> activeCharacters;

    CombatControl combat;
    SystemControl system;

protected:
    std::string characterName;
    int currentHealth;
    std::unique_ptr<RaceBase> race;
    std::unique_ptr<ClassBase> characterClass;
    Attributes finalStats;
    std::unique_ptr<Inventory> inventory;

    std::vector<std::unique_ptr<StatusEffect>> activeEffects;
    std::vector<std::unique_ptr<StatusEffect>> effectAdditionQueue;
    std::vector<EffectID> effectRemovalQueue;
    bool processingEffects = false;

    std::map<EquipmentSlot, Item*> equipment;
    Item* itemSelectedForUse;

    // Cache de getters calculados
    struct AttributeCache {
        int maxHealth = 0;
        int strength = 0;
        int dexterity = 0;
        int resistance = 0;
        int constitution = 0;
        int intelligence = 0;
        int wisdom = 0;
        int percentageReduction = 0;
        bool dirty = true;
    };
    mutable AttributeCache cache_;
    mutable std::mutex mutexCache_; // Protege o acesso ao cache em ambientes multithread

    struct DamageCache {
        std::pair<int, int> physicalMagicalDamage = {0, 0};
        bool dirty = true;
        
        void invalidate() { dirty = true; }
    };
    mutable DamageCache cacheDamage_;

    void updateCacheIfNeeded() const;
    
    std::unique_ptr<LevelSystem> levelSystem;

    int* getStaticAttributePointer(AttributeType attribute);

public:
    Character(const Character& other);
    Character(const std::string& name, std::unique_ptr<RaceBase> r, std::unique_ptr<ClassBase> c);
    virtual ~Character();

    static bool isValid(Character* p);
    static bool isValido(Character* p) { return isValid(p); }

    void calculateAttributes();
    void calcularAtributos() { calculateAttributes(); }

    std::unique_ptr<Character> clone() const;
    void showStatus() const;
    void mostrarStatus() const { showStatus(); }

    void modifyHealth(int value);
    void modificarVida(int valor) { modifyHealth(valor); }

    void changeName(const std::string& newName) { characterName = newName; }
    void alterarNome(const std::string& novoNome) { changeName(novoNome); }

    void equipItem(Item* item);
    void equiparItem(Item* item) { equipItem(item); }

    std::string getName() const { return characterName; }
    int getHealth() const { return currentHealth; }
    int obterVida() const { return getHealth(); }

    int getMaxHealth() const {
        if (combat.fixedMaxHealth > 0) return combat.fixedMaxHealth;
        updateCacheIfNeeded(); return cache_.maxHealth;
    }
    int obterVidaMaxima() const { return getMaxHealth(); }

    int getStrength() const { updateCacheIfNeeded(); return cache_.strength; }
    int getDexterity() const { updateCacheIfNeeded(); return cache_.dexterity; }
    int getResistance() const { updateCacheIfNeeded(); return cache_.resistance; }
    int getConstitution() const { updateCacheIfNeeded(); return cache_.constitution; }
    int getIntelligence() const { updateCacheIfNeeded(); return cache_.intelligence; }
    int getInteligencia() const { return getIntelligence(); }
    int getWisdom() const { updateCacheIfNeeded(); return cache_.wisdom; }
    
    int getLevel() const { return levelSystem->getLevel(); }
    int getCurrentXp() const { return levelSystem->getCurrentXp(); }
    int getXpAtual() const { return getCurrentXp(); }
    int getRequiredXpForLevelUp() const { return levelSystem->getXpToLevelUp(); }
    int getXpParaSubir() const { return getRequiredXpForLevelUp(); }

    void setLevel(int newLevel) { levelSystem->setLevel(newLevel); }
    void definirNivel(int novoNivel) { setLevel(novoNivel); }
    void setCurrentXp(int newXp) { levelSystem->setCurrentXp(newXp); }
    void definirXpAtual(int novoXp) { setCurrentXp(novoXp); }
    void setRequiredXpForLevelUp(int newXp) { levelSystem->setXpToLevelUp(newXp); }
    void definirXpParaSubir(int novoXpParaSubir) { setRequiredXpForLevelUp(novoXpParaSubir); }
    void setHealth(int newHealth) { currentHealth = newHealth; }
    void definirVida(int novaVida) { setHealth(novaVida); }
    void gainXp(int amount) { levelSystem->addXp(amount); }
    void ganharXp(int valor) { gainXp(valor); }
    void addXp(int xp) { gainXp(xp); }
    bool canLevelUp() const { return levelSystem->canLevelUp(); }
    bool podeSubirDeNivel() const { return canLevelUp(); }

    bool levelUp(AttributeType attribute);
    bool subirDeNivel(AttributeType atributo) { return levelUp(atributo); }

    void scaleAttributes(double factor);
    void escalarAtributos(double fator) { scaleAttributes(fator); }

    void addSoul(std::unique_ptr<Character> soul);
    void adicionarAlma(std::unique_ptr<Character> alma) { addSoul(std::move(alma)); }

    std::vector<std::unique_ptr<Character>>& getSouls();
    std::vector<std::unique_ptr<Character>>& obterAlmas() { return getSouls(); }

    size_t getSoulCount() const;
    size_t obterNumeroDeAlmas() const { return getSoulCount(); }

    std::unique_ptr<Character> removeSoul(int index);
    std::unique_ptr<Character> removerAlma(int index) { return removeSoul(index); }

    void forceCacheRecalculation() { cache_.dirty = true; }
    void strengthrRecalculoCache() { forceCacheRecalculation(); }
    
    int getTotalHealingReceived() const { return combat.totalHealingReceived; }
    int obterCuraTotalRecebida() const { return getTotalHealingReceived(); }

    void modifyStaticAttribute(AttributeType attribute, int value);
    void alterStaticAttribute(AttributeType attribute, int value) { modifyStaticAttribute(attribute, value); }
    void alterarAtributoEstatico(AttributeType atributo, int valor) { modifyStaticAttribute(atributo, valor); }

    Attributes& getFinalAttributes() { return finalStats; }
    Attributes& getFinalStats() { return finalStats; }
    const Attributes& getFinalStats() const { return finalStats; }
    Attributes& obterAtributosFinais() { return getFinalAttributes(); }

    RaceBase* getRace() const;
    RaceBase* obterRaca() const { return getRace(); }

    ClassBase* getClass() const;
    ClassBase* obterClasse() const { return getClass(); }

    std::string getClassName() const;
    std::string getNameClasse() const { return getClassName(); }

    ClassType getClassType() const;
    ClassType obterClassType() const { return getClassType(); }

    RaceType getRaceType() const;
    RaceType obterRaceType() const { return getRaceType(); }

    bool isBoss() const;
    
    Item* getEquippedAt(EquipmentSlot slot) const { auto it = equipment.find(slot); return it != equipment.end() ? it->second : nullptr; }
    Item* getWeapon() const { return getEquippedAt(EquipmentSlot::MainHand); }
    Item* obterArma() const { return getWeapon(); }
    Item* getShield() const { return getEquippedAt(EquipmentSlot::OffHand); }
    Item* obterEscudo() const { return getShield(); }
    Item* getArmor() const { return getEquippedAt(EquipmentSlot::Armor); }
    Item* obterArmadura() const { return getArmor(); }
    Item* getQuickConsumable() const { return getEquippedAt(EquipmentSlot::Consumable); }
    Item* obterConsumivelRapido() const { return getQuickConsumable(); }

    void unequipConsumable() { equipment.erase(EquipmentSlot::Consumable); cache_.dirty = true; }
    void desequiparConsumivel() { unequipConsumable(); }
    void unequipShield() { equipment.erase(EquipmentSlot::OffHand); cache_.dirty = true; }
    void desequiparEscudo() { unequipShield(); }
    void unequipWeapon() { equipment.erase(EquipmentSlot::MainHand); cache_.dirty = true; }
    void desequiparArma() { unequipWeapon(); }
    void unequipArmor() { equipment.erase(EquipmentSlot::Armor); cache_.dirty = true; }
    void desequiparArmadura() { unequipArmor(); }

    Inventory* getInventory() const { return inventory.get(); }
    Inventory* obterInventario() const { return getInventory(); }

    Item* getItemSelectedForUse() const { return itemSelectedForUse; }
    Item* obterItemSelecionadoParaUso() const { return getItemSelectedForUse(); }
    void setItemSelectedForUse(Item* item) { itemSelectedForUse = item; }
    void definirItemSelecionadoParaUso(Item* item) { setItemSelectedForUse(item); }

    bool isItemEquipped(Item* item) const { 
        if (!item) return false;
        for (const auto& pair : equipment) {
            if (pair.second == item) return true;
        }
        return false;
    }
    bool isItemEquipado(Item* item) const { return isItemEquipped(item); }

    bool isInCombat() const;
    bool estaEmCombate() const { return isInCombat(); }
    void enterCombat();
    void entrarEmCombate() { enterCombat(); }
    void exitCombat();
    void sairDoCombate() { exitCombat(); }
    void prepareForCombat();
    void prepararParaCombate() { prepareForCombat(); }

    void addGold(int amount) { inventory->addGold(amount); }
    void ganharOuro(int valor) { addGold(valor); }

    void setMultiplier(double newMultiplier);
    void definirMultiplicador(double novoMultiplicador) { setMultiplier(novoMultiplicador); }
    double getMultiplier() const { return combat.currentMultiplier; }
    double obterMultiplicador() const { return getMultiplier(); }

    bool canUseResurrection() const { return system.canRevive; }
    bool podeUsarRessurreicao() const { return canUseResurrection(); }
    void consumeResurrection() { system.canRevive = false; }
    void consumirRessurreicao() { consumeResurrection(); }

    int getAbilityCooldown(AbilityID ability) const 
    {
        auto it{combat.activeCooldowns.find(ability)};
        return (it != combat.activeCooldowns.end()) ? it->second : 0;
    }
    int obterRecargaHabilidade(AbilityID habilidade) const { return getAbilityCooldown(habilidade); }

    void setCooldown(AbilityID ability, int turns) 
    {
        combat.activeCooldowns[ability] = turns;
    }
    void definirCooldown(AbilityID habilidade, int turnos) { setCooldown(habilidade, turnos); }
    
    bool getAbilityCanceled() const { return combat.abilityCanceled; }
    bool obterHabilidadeCancelada() const { return getAbilityCanceled(); }
    void setAbilityCanceled(bool wasCanceled) { combat.abilityCanceled = wasCanceled; }
    void definirHabilidadeCancelada(bool foiCancelada) { setAbilityCanceled(foiCancelada); }

    void setCooldownState(bool onCooldown) { combat.abilityCooldown = onCooldown; }
    void definirRecarga(bool emRecarga) { setCooldownState(emRecarga); }
    bool getCooldownState() const { return combat.abilityCooldown; }
    bool obterRecarga() const { return getCooldownState(); }

    void setSkipEnemyTurn(bool skipTurn) { combat.skipEnemyTurn = skipTurn; }
    void definirPularTurnoInimigo(bool pularTurno) { setSkipEnemyTurn(pularTurno); }
    bool getSkipEnemyTurn() const { return combat.skipEnemyTurn; }
    bool obterPularTurnoInimigo() const { return getSkipEnemyTurn(); }
    
    void setReturnToMenu(bool returnToMenu) { system.wantsToReturnToMenu = returnToMenu; }
    void definirVoltarProMenu(bool voltar) { setReturnToMenu(voltar); }
    bool shouldReturnToMenu() const { return system.wantsToReturnToMenu; }
    bool obterVoltarProMenu() const { return shouldReturnToMenu(); }

    void unlockLabyrinth() { system.labyrinthUnlocked = true; }
    void desbloquearLabirinto() { unlockLabyrinth(); }
    bool isLabyrinthUnlocked() const { return system.labyrinthUnlocked; }
    bool obterLabirintoDesbloqueado() const { return isLabyrinthUnlocked(); }

    void unlockTrollRegeneration() { system.hasTrollRegeneration = true; }
    void desbloquearRegeneracaoTroll() { unlockTrollRegeneration(); }
    bool hasTrollRegeneration() const { return system.hasTrollRegeneration; }
    bool possuiRegeneracaoTroll() const { return hasTrollRegeneration(); }

    void toggleGodMode() { system.godModeActive = !system.godModeActive; }
    void alternarGodMode() { toggleGodMode(); }
    bool isGodMode() const { return system.godModeActive; }

    void toggleNoclip() { system.noclipActive = !system.noclipActive; }
    void alternarNoclip() { toggleNoclip(); }
    bool isNoclip() const { return system.noclipActive; }

    void reduceCooldowns();
    void reduzirCooldowns() { reduceCooldowns(); }

    void prepareForNewBattle();
    void prepararParaNovaBatalha() { prepareForNewBattle(); }

    void finishBattle();
    void finalizarBatalha() { finishBattle(); }

    bool hasEffect(EffectID id) const;
    bool possuiEfeito(EffectID id) const { return hasEffect(id); }

    int getEffectTurns(EffectID id) const;
    int obterTurnosEfeito(EffectID id) const { return getEffectTurns(id); }

    const StatusEffect* findEffect(EffectID id) const;
    const StatusEffect* encontrarEfeito(EffectID id) const { return findEffect(id); }

    void setDefending(bool d) { combat.isDefending = d; }
    void definirDefendendo(bool d) { setDefending(d); }
    bool isDefending() const { return combat.isDefending; }
    bool obterDefendendo() const { return isDefending(); }

    void setDefenseCooldown(bool r) { combat.defenseCooldown = r; }
    void definirRecargaDefesa(bool r) { setDefenseCooldown(r); }
    bool getDefenseCooldown() const { return combat.defenseCooldown; }
    bool obterRecargaDefesa() const { return getDefenseCooldown(); }

    void setAnimatedDeath(bool m) { combat.animatedDeath = m; }
    void definirMorteAnimada(bool m) { setAnimatedDeath(m); }
    bool getAnimatedDeath() const { return combat.animatedDeath; }
    bool obterMorteAnimada() const { return getAnimatedDeath(); }

    void setParryEnabled(bool p) { system.parryEnabled = p; }
    void definirParryAtivado(bool p) { setParryEnabled(p); }
    bool isParryEnabled() const { return system.parryEnabled; }
    bool obterParryAtivado() const { return isParryEnabled(); }

    void setModernParry(bool m) { system.modernParry = m; }
    void definirParryModerno(bool m) { setModernParry(m); }
    bool isModernParry() const { return system.modernParry; }
    bool obterParryModerno() const { return isModernParry(); }

    void setAsMinion(bool minion) { system.isMinion = minion; }
    bool isMinion() const { return system.isMinion; }

    void setDifficulty(GameDifficulty d) { system.currentDifficulty = d; }
    void definirDificuldade(GameDifficulty d) { setDifficulty(d); }
    GameDifficulty getDifficulty() const { return system.currentDifficulty; }
    GameDifficulty obterDificuldade() const { return getDifficulty(); }

    void applyDifficultyMultiplier(double mult);
    void aplicarMultiplicadorDificuldade(double mult) { applyDifficultyMultiplier(mult); }

    void setPlayerIcon(char icon) { system.playerIcon = icon; }
    void definirIconeJogador(char icone) { setPlayerIcon(icone); }
    char getPlayerIcon() const { return system.playerIcon; }
    char obterIconeJogador() const { return getPlayerIcon(); }

    void setPlayerColor(Color color) { system.playerColor = color; }
    void definirCorJogador(Color cor) { setPlayerColor(cor); }
    Color getPlayerColor() const { return system.playerColor; }
    Color obterCorJogador() const { return getPlayerColor(); }

    void setTerminalBackgroundColor(Color color) { system.terminalBackgroundColor = color; }
    void definirCorFundoTerminal(Color cor) { setTerminalBackgroundColor(cor); }
    Color getTerminalBackgroundColor() const { return system.terminalBackgroundColor; }
    Color obterCorFundoTerminal() const { return getTerminalBackgroundColor(); }

    AttackType getAttackType() const;
    bool classAbilityConsumesTurn() const;
    bool habilidadeDaClasseConsomeTurno() const { return classAbilityConsumesTurn(); }

    void addEffect(std::unique_ptr<StatusEffect> effect);
    void adicionarEfeito(std::unique_ptr<StatusEffect> efeito) { addEffect(std::move(efeito)); }

    void processTurnStartEffects();
    void processarEfeitosInicioTurno() { processTurnStartEffects(); }

    bool canAct(std::string& outIncapacityReason) const;
    bool podeAgir(std::string& outMotivoIncapacidade) const { return canAct(outMotivoIncapacidade); }

    // Preenche o vetor com os IDs de todos os efeitos ativos
    void getActiveEffectIDs(std::vector<EffectID>& outIDs) const;
    void obterIDsEfeitosAtivos(std::vector<EffectID>& outIDs) const { getActiveEffectIDs(outIDs); }

    void clearEffects();
    void limparEfeitos() { clearEffects(); }

    void removeEffect(EffectID id);
    void removerEfeito(EffectID id) { removeEffect(id); }

    DamageResult takeDamage(int rawDamage, int piercingDamage, int parryReducedDamage, IAttacker* attacker, bool applyPassives = true) override;
    DamageResult receberDano(int danoBruto, int danoPerfurante, int danoReduzidoParry, IAttacker* atacante, bool aplicarPassivas = true) override {
        return takeDamage(danoBruto, danoPerfurante, danoReduzidoParry, atacante, aplicarPassivas);
    }

    int calculateBaseDefense(int rawDamage, int piercingDamage) override;
    int calcularDefesaBase(int danoBruto, int danoPerfurante) override {
        return calculateBaseDefense(danoBruto, danoPerfurante);
    }

    std::pair<int, int> calculateBaseOffensiveDamage() override;
    std::pair<int, int> calcularDanoOfensivoBase() override {
        return calculateBaseOffensiveDamage();
    }

    int ensureMinimumDamage(int currentDamage) override;
    int garantirDanoMinimo(int danoAtual) override {
        return ensureMinimumDamage(danoAtual);
    }

    virtual void executeDrops(Character* currentPlayer, std::vector<std::string>& obtainedItems, int& totalGold, int& totalXp);
    virtual void executarDrops(Character* currentPlayer, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal) {
        executeDrops(currentPlayer, itensObtidos, ouroTotal, xpTotal);
    }
};

using Personagem = Character;
using Atributos = Attributes;
