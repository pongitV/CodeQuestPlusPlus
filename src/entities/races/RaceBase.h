#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../../systems/inventory/Item.h"
#include "../character/Character.h"

enum class RaceType 
{
    Nenhum,
    Dwarf,
    Elf,
    Human,
    Ork,
    OrkExilado,
    Goblin,
    Fairy,
    Slime,
    AbominacaoFloresta,
    Mimic,
    Troll,
    Mahoraga
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
    virtual RaceType obterRaceType() const = 0;
    virtual const std::vector<std::string>& getRaceAppearance() const {
        static const std::vector<std::string> vazia;
        return vazia;
    }
    virtual std::string getRaceSpritePath() const { return ""; }

    virtual Attributes getRaceAttributes() const = 0;

    virtual std::string getRaceAbilityName() const = 0;
    virtual std::string getRaceAbilityDescription() const = 0;

    virtual BestiaryInfo obterBestiaryInfo() const { return {"Desconhecido", "Desconhecido", "", "", {}, 1}; }

    virtual std::vector<std::unique_ptr<Item>> getRaceEquipment() const { return {}; }

    virtual int processOffensiveDamage(int baseDamage, Character* /*atacante*/) {
        return baseDamage;
    }

    virtual int processDefensiveDamage(int finalDamage, Character* /*defensor*/) {
        return finalDamage;
    }
    
    virtual void aoSofrerParryPerfeito() {}
    virtual bool ignoraParry() const { return false; }
    virtual bool ignoraEscudo() const { return false; }

    virtual void realizarDrops(Character* /*enemy*/, Character* /*currentPlayer*/, std::vector<std::string>& /*itensObtidos*/, int& /*ouroTotal*/, int& /*xpTotal*/) {
        // Implementação padrão vazia (sem drops)
    }

    virtual void aoCausarDano(Character* /*atacante*/, Character* /*alvo*/, int /*danoCausado*/) {}

    virtual bool tentarUsarHabilidadeAtiva(Character* /*esteInimigo*/, Character* /*alvo*/, int /*difficulty*/) {
        return false; // Por padrao, enemies nao possuem habilidades ativas que consomem o turno
    }
};

template<typename Derived>
class RaceBaseCRTP : public RaceBase {
public:
    Derived& derived() { return static_cast<Derived&>(*this); }
    const Derived& derived() const { return static_cast<const Derived&>(*this); }
};
