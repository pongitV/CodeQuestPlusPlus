#pragma once

#include "../character/Character.h"
#include "../../systems/inventory/Inventory.h"
#include "../../systems/inventory/Item.h"
#include "../../systems/inventory/ItemFactory.h"
#include <string>
#include <vector>
#include "../../core/utils/Color.h"
#include "../../core/utils/InputControl.h"
#include "../../ui/screens/combat/ScreenCombat.h"
#include <memory>
#include <thread>
#include <chrono>
#include <functional>
#include <iostream>
#include <algorithm>

enum class ClassType 
{
    None,
    Archer,
    Bard,
    Warrior,
    Mage,
    Necromancer,

    // Aliases para compatibilidade legada
    Nenhum = None,
    NECROMANTE = Necromancer
};

enum class AbilityID
{
    None,
    Determination,
    ArcaneChanneling,
    FlashingLights,
    OnSight,
    ThroughTheWire,
    RetreatWithAim,
    SummonSpecter,

    // Aliases para compatibilidade legada
    Nenhuma = None,
    Determinacao = Determination,
    CanalizacaoArcana = ArcaneChanneling,
    RetiradaComPontaria = RetreatWithAim,
    InvocacaoDeEspectro = SummonSpecter
};

class Character;
class Item;
class Combat;

class ClassBase 
{
public:
    virtual ~ClassBase() = default;

    // INFORMAÇÕES DA CLASSE
    virtual std::string getClassName() const = 0;
    virtual std::string getNameClasse() const { return getClassName(); }

    virtual ClassType getClassType() const = 0;
    virtual ClassType obterClassType() const { return getClassType(); }

    virtual const std::vector<std::string>& getClassMenuAppearance() const {
        static const std::vector<std::string> emptyList;
        return emptyList;
    }
    virtual const std::vector<std::string>& obterAparenciaClasseMenu() const { return getClassMenuAppearance(); }

    virtual std::string getSpritePath() const { return ""; }
    virtual std::string obterCaminhoSprite() const { return getSpritePath(); }

    virtual Attributes getClassAttributes() const = 0;
    virtual Attributes obterAtributosClasse() const { return getClassAttributes(); }

    virtual std::vector<std::unique_ptr<Item>> getClassEquipment() const = 0;
    virtual std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const { return getClassEquipment(); }
 
    virtual std::string getClassPassiveName() const = 0;
    virtual std::string getNamePassivaClasse() const { return getClassPassiveName(); }

    virtual std::string getClassPassiveDescription() const = 0;
    virtual std::string obterDescricaoPassivaClasse() const { return getClassPassiveDescription(); }

    virtual std::string getClassAbilityCooldownDescription() const = 0;
    virtual std::string obterRecargaHabilidadeClasse() const { return getClassAbilityCooldownDescription(); }

    // HABILIDADE DA CLASSE
    virtual std::string getClassAbilityName() const = 0;
    virtual std::string getNameHabilidadeClasse() const { return getClassAbilityName(); }

    virtual std::string getClassAbilityDescription() const = 0;
    virtual void useClassAbility(Combat* combat, Character* userCharacter, std::vector<Character*>& enemyList) = 0;
    virtual AttackType getAttackType() const { return AttackType::Single; }
    virtual bool abilityConsumesTurn() const { return true; }

protected:
    void notifyCombatMessage(const std::string& msgWithColor, const std::string& msgNoColor) const {
        std::string text = msgNoColor.empty() ? msgWithColor : msgNoColor;
        TelaCombate::adicionarMensagemFixa(text);
    }
    void notificarMensagemCombate(const std::string& msgWithColor, const std::string& msgNoColor) const {
        notifyCombatMessage(msgWithColor, msgNoColor);
    }

    bool checkAndReportCooldown(Character* userCharacter, int remainingTurns, const std::string& /*abilityName*/) const {
        if (remainingTurns > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            userCharacter->definirHabilidadeCancelada(true);
            return true;
        }
        return false;
    }
    bool verificarEReportarRecarga(Character* userCharacter, int remainingTurns, const std::string& abilityName) const {
        return checkAndReportCooldown(userCharacter, remainingTurns, abilityName);
    }

public:
    // PASSIVAS DE CLASSE
    virtual int processBardPassiveHealing(int baseHeal) const { return baseHeal; }
    virtual int processarCuraPassivaBard(int curaBase) const { return processBardPassiveHealing(curaBase); }

    virtual double processBardPassiveBuffMultiplier(double baseMultiplier) const { return baseMultiplier; }
    virtual double processarMultiplicadorBuffPassivaBard(double multBase) const { return processBardPassiveBuffMultiplier(multBase); }

    virtual int processArcherPassiveArmorPenalty(int basePenalty) const { return basePenalty; }
    virtual int processarPenalidadeArmaduraPassivaArqueiro(int penalidadeBase) const { return processArcherPassiveArmorPenalty(penalidadeBase); }

    virtual int applyArcherPassiveSlowPenalty(int currentDexterity) const { return currentDexterity / 2; }
    virtual int aplicarPenalidadeLentidaoPassivaArqueiro(int dexterityAtual) const { return applyArcherPassiveSlowPenalty(dexterityAtual); }

    virtual int revertArcherPassiveSlowPenalty(int currentDexterity) const { return currentDexterity * 2; }
    virtual int reverterPenalidadeLentidaoPassivaArqueiro(int dexterityAtual) const { return revertArcherPassiveSlowPenalty(dexterityAtual); }

    // PROCESSAMENTO DE DANO
    virtual void executeAttackWithClassPassive(Character* attacker, Character* defender, int baseDamage, int piercingDamage, std::vector<std::unique_ptr<Character>>& enemies, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool isAttackerPlayer) {
        baseDamage = processPreAttackDamage(attacker, defender, baseDamage, isAttackerPlayer, enemies.size());

        bool isArea = attacker->getAttackType() == AttackType::Area && isAttackerPlayer && !enemies.empty();

        if (isArea) {
            executeAreaAttack(attacker, defender, baseDamage, piercingDamage, enemies, applyDamage, isAttackerPlayer);
        } else {
            executeSingleAttack(attacker, defender, baseDamage, piercingDamage, enemies, applyDamage, isAttackerPlayer);
        }
    }

    void executarAtaqueComPassivaDaClasse(Character* atacante, Character* defensor, int baseDamage, int danoPerfurante, std::vector<std::unique_ptr<Character>>& enemies, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool isAtacanteJogador) {
        executeAttackWithClassPassive(atacante, defensor, baseDamage, danoPerfurante, enemies, applyDamage, isAtacanteJogador);
    }

protected:
    virtual void executeAreaAttack(Character* attacker, Character* defender, int baseDamage, int piercingDamage, std::vector<std::unique_ptr<Character>>& enemies, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool isAttackerPlayer) {
        int splitDamage = std::max(1, baseDamage / static_cast<int>(enemies.size()));
        int splitPiercing = piercingDamage / static_cast<int>(enemies.size());

        bool triggeredPassive = false;
        for (auto& currentEnemy : enemies) {
            applyDamage(attacker, currentEnemy.get(), splitDamage, splitPiercing);
            processPostAttackDamage(attacker, currentEnemy.get(), defender, baseDamage, piercingDamage, applyDamage, isAttackerPlayer, true, triggeredPassive);
        }
    }

    virtual void executeSingleAttack(Character* attacker, Character* defender, int baseDamage, int piercingDamage, std::vector<std::unique_ptr<Character>>& enemies, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool isAttackerPlayer) {
        if (defender != nullptr) {
            applyDamage(attacker, defender, baseDamage, piercingDamage);
        }

        bool triggeredPassive = false;
        for (auto& currentEnemy : enemies) {
            processPostAttackDamage(attacker, currentEnemy.get(), defender, baseDamage, piercingDamage, applyDamage, isAttackerPlayer, false, triggeredPassive);
        }
    }

    virtual int processPreAttackDamage(Character* /*attacker*/, Character* /*defender*/, int baseDamage, bool /*isAttackerPlayer*/, size_t /*enemyCount*/) { return baseDamage; }
    virtual void processPostAttackDamage(Character* /*attacker*/, Character* /*currentTarget*/, Character* /*mainDefender*/, int /*baseDamage*/, int /*piercingDamage*/, const std::function<void(Character*, Character*, int, int)>& /*applyDamage*/, bool /*isAttackerPlayer*/, bool /*isArea*/, bool& /*triggeredPassive*/) {}

    // Delegados legados
    virtual void executarAtaqueArea(Character* atacante, Character* defensor, int baseDamage, int danoPerfurante, std::vector<std::unique_ptr<Character>>& enemies, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool isAtacanteJogador) {
        executeAreaAttack(atacante, defensor, baseDamage, danoPerfurante, enemies, applyDamage, isAtacanteJogador);
    }
    virtual void executarAtaqueUnico(Character* atacante, Character* defensor, int baseDamage, int danoPerfurante, std::vector<std::unique_ptr<Character>>& enemies, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool isAtacanteJogador) {
        executeSingleAttack(atacante, defensor, baseDamage, danoPerfurante, enemies, applyDamage, isAtacanteJogador);
    }
    virtual int processarDanoPreAtaque(Character* atacante, Character* defensor, int baseDamage, bool isAtacanteJogador, size_t qtdInimigos) {
        return processPreAttackDamage(atacante, defensor, baseDamage, isAtacanteJogador, qtdInimigos);
    }
    virtual void processarDanoPosAtaque(Character* atacante, Character* alvoAtual, Character* defensorPrincipal, int baseDamage, int danoPerfurante, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool isAtacanteJogador, bool isArea, bool& ativouPassiva) {
        processPostAttackDamage(atacante, alvoAtual, defensorPrincipal, baseDamage, danoPerfurante, applyDamage, isAtacanteJogador, isArea, ativouPassiva);
    }
};

template<typename Derived>
class ClassBaseCRTP : public ClassBase {
public:
    Derived& derived() { return static_cast<Derived&>(*this); }
    const Derived& derived() const { return static_cast<const Derived&>(*this); }
};
