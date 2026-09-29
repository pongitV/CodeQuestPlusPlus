#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../../systems/inventory/Item.h"
#include "../character/Character.h"

enum class RaceType 
{
    None,
    Dwarf,
    Elf,
    Human,
    Orc,
    ExiledOrc,
    Goblin,
    Fairy,
    Slime,
    ForestAbomination,
    Mimic,
    Troll,
    Mahoraga,

    // Aliases para compatibilidade legada
    Nenhum = None,
    Ork = Orc,
    OrkExilado = ExiledOrc,
    AbominacaoFloresta = ForestAbomination
};

struct BestiaryInfo {
    std::string map;
    std::string habitat;
    std::string lore;
    std::string funFact;
    std::vector<std::string> drops;
    int difficulty;
};

class RaceBase {
public:
    virtual ~RaceBase() = default;

    virtual std::string getRaceName() const = 0;
    virtual RaceType getRaceType() const = 0;
    virtual RaceType obterRaceType() const { return getRaceType(); }

    virtual const std::vector<std::string>& getRaceAppearance() const {
        static const std::vector<std::string> emptyList;
        return emptyList;
    }
    virtual std::string getRaceSpritePath() const { return ""; }

    virtual Attributes getRaceAttributes() const = 0;

    virtual std::string getRaceAbilityName() const = 0;
    virtual std::string getRaceAbilityDescription() const = 0;

    virtual BestiaryInfo getBestiaryInfo() const { return {"Desconhecido", "Desconhecido", "", "", {}, 1}; }
    virtual BestiaryInfo obterBestiaryInfo() const { return getBestiaryInfo(); }

    virtual std::vector<std::unique_ptr<Item>> getRaceEquipment() const { return {}; }

    virtual int processOffensiveDamage(int baseDamage, Character* /*atacante*/) {
        return baseDamage;
    }

    virtual int processDefensiveDamage(int finalDamage, Character* /*defensor*/) {
        return finalDamage;
    }
    
    virtual void onSufferingPerfectParry() {}
    virtual void onPerfectParrySuffered() { onSufferingPerfectParry(); }
    virtual void aoSofrerParryPerfeito() { onSufferingPerfectParry(); }

    virtual bool ignoresParry() const { return false; }
    virtual bool ignoraParry() const { return ignoresParry(); }

    virtual bool ignoresShield() const { return false; }
    virtual bool ignoraEscudo() const { return ignoresShield(); }

    virtual void performDrops(Character* /*inimigo*/, Character* /*jogadorAtual*/, std::vector<std::string>& /*itensObtidos*/, int& /*totalOuro*/, int& /*totalXP*/) {}
    virtual void dropLoot(Character* enemy, Character* currentPlayer, std::vector<std::string>& items, int& gold, int& xp) {
        performDrops(enemy, currentPlayer, items, gold, xp);
    }
    virtual void realizarDrops(Character* enemy, Character* currentPlayer, std::vector<std::string>& items, int& gold, int& xp) {
        performDrops(enemy, currentPlayer, items, gold, xp);
    }
    virtual void executeDrops(Character* enemy, Character* currentPlayer, std::vector<std::string>& items, int& gold, int& xp) {
        performDrops(enemy, currentPlayer, items, gold, xp);
    }

    virtual void onDealingDamage(Character* /*atacante*/, Character* /*alvo*/, int /*danoCausado*/) {}
    virtual void onDamageDealt(Character* attacker, Character* target, int damage) {
        onDealingDamage(attacker, target, damage);
    }
    virtual void aoCausarDano(Character* attacker, Character* target, int damage) {
        onDealingDamage(attacker, target, damage);
    }

    virtual bool tryUseActiveAbility(Character* /*esteInimigo*/, Character* /*alvo*/, int /*dificuldade*/) {
        return false; // Por padrao, inimigos nao possuem habilidades ativas que consomem o turno
    }
    virtual bool tentarUsarHabilidadeAtiva(Character* thisEnemy, Character* target, int diff) {
        return tryUseActiveAbility(thisEnemy, target, diff);
    }
};

template<typename Derived>
class RaceBaseCRTP : public RaceBase {
public:
    Derived& derived() { return static_cast<Derived&>(*this); }
    const Derived& derived() const { return static_cast<const Derived&>(*this); }
};
