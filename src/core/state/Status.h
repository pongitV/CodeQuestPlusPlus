#pragma once

#include <string>

// Enum que representa os identificadores de efeitos de status ativos
enum class EffectID {
    None,
    Bleeding,
    Slow,
    Weakness,
    ArmorBreak,
    AdaptationWheel,
    Inviolable,
    HalfDamage,
    LifeSteal,
    Stun,
    AttributeBuff,
    WarCry,
    Necrosis,
    TrueAim,

    // Constantes de enum para compatibilidade retroativa
    Nenhum = None,
    Sangramento = Bleeding,
    Lentidao = Slow,
    Fraqueza = Weakness,
    QuebraResistencia = ArmorBreak,
    RodaAdaptacao = AdaptationWheel,
    Inviolavel = Inviolable,
    MetadeDano = HalfDamage,
    SugaSangue = LifeSteal,
    Atordoamento = Stun,
    BuffAtributos = AttributeBuff,
    GritoDeGuerra = WarCry,
    Necrose = Necrosis,
    MiraCerteira = TrueAim
};

// Alias de compatibilidade legada
using EfeitoID = EffectID;

class Character;

// BaseStatus: Classe base abstrata que representa efeitos de status aplicados a personagens.
class BaseStatus {
protected:
    EffectID id;
    std::string name;
    int remainingTurns;
public:
    BaseStatus(EffectID id, const std::string& effectName, int durationTurns) 
        : id(id), name(effectName), remainingTurns(durationTurns) {}
    virtual ~BaseStatus() = default;

    EffectID getId() const { return id; }
    std::string getName() const { return name; }
    int getRemainingTurns() const { return remainingTurns; }
    void decrementTurn() { if (remainingTurns > 0) remainingTurns--; }
    bool hasExpired() const { return remainingTurns <= 0; }

    virtual void onEnter(Character* target) {}
    virtual void applyTurnStart(Character* target) {}
    virtual void onExit(Character* target) {}

    virtual int processIncomingDamage(int damage) { return damage; }
    virtual bool preventsAction() const { return false; }

    // Metodos legados para compatibilidade retroativa
    EffectID obterID() const { return getId(); }
    int obterTurnosRestantes() const { return getRemainingTurns(); }
    void decrementarTurno() { decrementTurn(); }
    bool expirou() const { return hasExpired(); }

    virtual void aoEntrar(Character* target) { onEnter(target); }
    virtual void aplicarInicioTurno(Character* target) { applyTurnStart(target); }
    virtual void aoSair(Character* target) { onExit(target); }
    virtual int processarDanoRecebido(int damage) { return processIncomingDamage(damage); }
    virtual bool impedeAcao() const { return preventsAction(); }

    void DecrementTurn() { decrementTurn(); }
    bool HasExpired() const { return hasExpired(); }
    virtual void ApplyStart(Character* target) { onEnter(target); }
    virtual void ApplyEnd(Character* target) { onExit(target); }
    virtual int ApplyDamage(int damage) { return processIncomingDamage(damage); }
};

class StatusEffect : public BaseStatus {
public:
    StatusEffect(EffectID id, const std::string& effectName, int durationTurns)
        : BaseStatus(id, effectName, durationTurns) {}
};

using EfeitoStatus = StatusEffect;

class StunEffect : public StatusEffect {
public:
    StunEffect(int durationTurns) : StatusEffect(EffectID::Stun, "Stun", durationTurns) {}
    bool preventsAction() const override { return true; }
};

using EfeitoAtordoamento = StunEffect;

class LifeStealEffect : public StatusEffect {
private:
    Character* attacker;
public:
    LifeStealEffect(int durationTurns, Character* attackingCharacter) 
        : StatusEffect(EffectID::LifeSteal, "LifeSteal", durationTurns), attacker(attackingCharacter) {}
    void applyTurnStart(Character* target) override;
};

using EfeitoSugaSangue = LifeStealEffect;

class SlowEffect : public StatusEffect {
public:
    SlowEffect(int durationTurns) : StatusEffect(EffectID::Slow, "Slow", durationTurns) {}
    void onEnter(Character* target) override;
    void onExit(Character* target) override;
};

using EfeitoLentidao = SlowEffect;

class WeaknessEffect : public StatusEffect {
private:
    int lostStrength;
public:
    WeaknessEffect(int durationTurns) : StatusEffect(EffectID::Weakness, "Weakness", durationTurns), lostStrength(0) {}
    void onEnter(Character* target) override;
    void onExit(Character* target) override;
};

using EfeitoFraqueza = WeaknessEffect;

class ArmorBreakEffect : public StatusEffect {
private:
    int lostResistance;
    int lostConstitution;
public:
    ArmorBreakEffect() : StatusEffect(EffectID::ArmorBreak, "ArmorBreak", 9999), lostResistance(0), lostConstitution(0) {}
    void onEnter(Character* target) override;
    void onExit(Character* target) override;
    void applyTurnStart(Character* target) override;
};

using EfeitoQuebraResistencia = ArmorBreakEffect;

class BleedingEffect : public StatusEffect {
private:
    int damagePerTurn;
public:
    BleedingEffect(int durationTurns, int damage) : StatusEffect(EffectID::Bleeding, "Bleeding", durationTurns), damagePerTurn(damage) {}
    void applyTurnStart(Character* target) override;
};

using EfeitoSangramento = BleedingEffect;

class NecrosisEffect : public StatusEffect {
private:
    int damagePerTurn;
public:
    NecrosisEffect(int durationTurns, int damage) : StatusEffect(EffectID::Necrosis, "Necrosis", durationTurns), damagePerTurn(damage) {}
    void applyTurnStart(Character* target) override;
};

using EfeitoNecrose = NecrosisEffect;

class HalfDamageEffect : public StatusEffect {
public:
    HalfDamageEffect(int durationTurns) : StatusEffect(EffectID::HalfDamage, "HalfDamage", durationTurns) {}
    int processIncomingDamage(int damage) override;
};

using EfeitoMetadeDano = HalfDamageEffect;

class AttributeBuffEffect : public StatusEffect {
public:
    AttributeBuffEffect(int durationTurns) : StatusEffect(EffectID::AttributeBuff, "AttributeBuff", durationTurns) {}
    void onExit(Character* target) override;
};

using EfeitoBuffAtributos = AttributeBuffEffect;

class InviolableEffect : public StatusEffect {
public:
    InviolableEffect(int durationTurns) : StatusEffect(EffectID::Inviolable, "Inviolable", durationTurns) {}
    void onExit(Character* target) override;
};

using EfeitoInviolavel = InviolableEffect;

class TrueAimEffect : public StatusEffect {
public:
    TrueAimEffect(int durationTurns) : StatusEffect(EffectID::TrueAim, "TrueAim", durationTurns) {}
};

using EfeitoMiraCerteira = TrueAimEffect;

class WarCryEffect : public StatusEffect {
private:
    int strengthBonus;
    int dexterityBonus;
public:
    WarCryEffect(int durationTurns, int strBonus, int dexBonus) 
        : StatusEffect(EffectID::WarCry, "WarCry", durationTurns), strengthBonus(strBonus), dexterityBonus(dexBonus) {}
    void onEnter(Character* target) override;
    void onExit(Character* target) override;
};

using EfeitoGritoGuerra = WarCryEffect;

class AdaptationWheelEffect : public StatusEffect {
private:
    int bonusStrength = 0;
    int bonusDexterity = 0;
    int bonusResistance = 0;
    int bonusConstitution = 0;
    int bonusIntelligence = 0;
    int bonusWisdom = 0;
public:
    AdaptationWheelEffect() : StatusEffect(EffectID::AdaptationWheel, "Divine Adaptation", 9999) {}
    bool preventsAction() const override { return false; }
    void applyTurnStart(Character* target) override;
    void onExit(Character* target) override;
    void adapt(Character* target, Character* enemy);
    void adaptar(Character* target, Character* enemy) { adapt(target, enemy); }
};

using EfeitoRodaAdaptacao = AdaptationWheelEffect;
