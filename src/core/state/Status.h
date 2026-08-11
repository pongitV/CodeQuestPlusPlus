#pragma once

#include <string>

// Enum representing active status effect identifiers
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

    // Backward-compatibility enum constants
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

// Legacy compatibility alias
using EfeitoID = EffectID;

class Character;

// BaseStatus: Abstract base class representing status effects applied to characters.
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

    virtual void onEnter(Character* target) { aoEntrar(target); }
    virtual void applyTurnStart(Character* target) { aplicarInicioTurno(target); }
    virtual void onExit(Character* target) { aoSair(target); }

    virtual int processIncomingDamage(int damage) { return processarDanoRecebido(damage); }
    virtual bool preventsAction() const { return impedeAcao(); }

    // Legacy method delegates
    EffectID obterID() const { return getId(); }
    int obterTurnosRestantes() const { return getRemainingTurns(); }
    void decrementarTurno() { decrementTurn(); }
    bool expirou() const { return hasExpired(); }

    virtual void aoEntrar(Character* /*target*/) {}
    virtual void aplicarInicioTurno(Character* /*target*/) {}
    virtual void aoSair(Character* /*target*/) {}
    virtual int processarDanoRecebido(int damage) { return damage; }
    virtual bool impedeAcao() const { return false; }

    void DecrementTurn() { decrementTurn(); }
    bool HasExpired() const { return hasExpired(); }
    virtual void ApplyStart(Character* target) { onEnter(target); }
    virtual void ApplyEnd(Character* target) { onExit(target); }
    virtual int ApplyDamage(int damage) { return processIncomingDamage(damage); }
};

class EfeitoStatus : public BaseStatus {
public:
    EfeitoStatus(EffectID id, const std::string& effectName, int durationTurns)
        : BaseStatus(id, effectName, durationTurns) {}
};

using StatusEffect = EfeitoStatus;

class EfeitoAtordoamento : public EfeitoStatus {
public:
    EfeitoAtordoamento(int durationTurns) : EfeitoStatus(EffectID::Stun, "Stun", durationTurns) {}
    bool preventsAction() const override { return true; }
    bool impedeAcao() const override { return preventsAction(); }
};

class EfeitoSugaSangue : public EfeitoStatus {
private:
    Character* atacante;
public:
    EfeitoSugaSangue(int durationTurns, Character* personagemAtacante) : EfeitoStatus(EffectID::LifeSteal, "LifeSteal", durationTurns), atacante(personagemAtacante) {}
    void applyTurnStart(Character* target) override { aplicarInicioTurno(target); }
    void aplicarInicioTurno(Character* target) override;
};

class EfeitoLentidao : public EfeitoStatus {
public:
    EfeitoLentidao(int durationTurns) : EfeitoStatus(EffectID::Slow, "Slow", durationTurns) {}
    void onEnter(Character* target) override { aoEntrar(target); }
    void onExit(Character* target) override { aoSair(target); }
    void aoEntrar(Character* target) override;
    void aoSair(Character* target) override;
};

class EfeitoFraqueza : public EfeitoStatus {
private:
    int strengthPerdida;
public:
    EfeitoFraqueza(int durationTurns) : EfeitoStatus(EffectID::Weakness, "Weakness", durationTurns), strengthPerdida(0) {}
    void onEnter(Character* target) override { aoEntrar(target); }
    void onExit(Character* target) override { aoSair(target); }
    void aoEntrar(Character* target) override;
    void aoSair(Character* target) override;
};

class EfeitoQuebraResistencia : public EfeitoStatus {
private:
    int resistancePerdida;
    int constitutionPerdida;
public:
    EfeitoQuebraResistencia() : EfeitoStatus(EffectID::ArmorBreak, "ArmorBreak", 9999), resistancePerdida(0), constitutionPerdida(0) {}
    void onEnter(Character* target) override { aoEntrar(target); }
    void onExit(Character* target) override { aoSair(target); }
    void applyTurnStart(Character* target) override { aplicarInicioTurno(target); }
    void aoEntrar(Character* target) override;
    void aoSair(Character* target) override;
    void aplicarInicioTurno(Character* target) override;
};

class EfeitoSangramento : public EfeitoStatus {
private:
    int danoPorTurno;
public:
    EfeitoSangramento(int durationTurns, int damage) : EfeitoStatus(EffectID::Bleeding, "Bleeding", durationTurns), danoPorTurno(damage) {}
    void applyTurnStart(Character* target) override { aplicarInicioTurno(target); }
    void aplicarInicioTurno(Character* target) override;
};

class EfeitoNecrose : public EfeitoStatus {
private:
    int danoPorTurno;
public:
    EfeitoNecrose(int durationTurns, int damage) : EfeitoStatus(EffectID::Necrosis, "Necrosis", durationTurns), danoPorTurno(damage) {}
    void applyTurnStart(Character* target) override { aplicarInicioTurno(target); }
    void aplicarInicioTurno(Character* target) override;
};

class EfeitoMetadeDano : public EfeitoStatus {
public:
    EfeitoMetadeDano(int durationTurns) : EfeitoStatus(EffectID::HalfDamage, "HalfDamage", durationTurns) {}
    int processIncomingDamage(int damage) override { return processarDanoRecebido(damage); }
    int processarDanoRecebido(int damage) override;
};

class EfeitoBuffAtributos : public EfeitoStatus {
public:
    EfeitoBuffAtributos(int durationTurns) : EfeitoStatus(EffectID::AttributeBuff, "AttributeBuff", durationTurns) {}
    void onExit(Character* target) override { aoSair(target); }
    void aoSair(Character* target) override;
};

class EfeitoInviolavel : public EfeitoStatus {
public:
    EfeitoInviolavel(int durationTurns) : EfeitoStatus(EffectID::Inviolable, "Inviolable", durationTurns) {}
    void onExit(Character* target) override { aoSair(target); }
    void aoSair(Character* target) override;
};

class EfeitoMiraCerteira : public EfeitoStatus {
public:
    EfeitoMiraCerteira(int durationTurns) : EfeitoStatus(EffectID::TrueAim, "TrueAim", durationTurns) {}
};

class EfeitoGritoGuerra : public EfeitoStatus {
private:
    int bonusForca;
    int bonusDestreza;
public:
    EfeitoGritoGuerra(int durationTurns, int strengthBonus, int dexterityBonus) : EfeitoStatus(EffectID::WarCry, "WarCry", durationTurns), bonusForca(strengthBonus), bonusDestreza(dexterityBonus) {}
    void onEnter(Character* target) override { aoEntrar(target); }
    void onExit(Character* target) override { aoSair(target); }
    void aoEntrar(Character* target) override;
    void aoSair(Character* target) override;
};

class EfeitoRodaAdaptacao : public EfeitoStatus {
private:
    int bForca = 0;
    int bDestreza = 0;
    int bResistencia = 0;
    int bConstituicao = 0;
    int bInteligencia = 0;
    int bSabedoria = 0;
public:
    EfeitoRodaAdaptacao() : EfeitoStatus(EffectID::AdaptationWheel, "Divine Adaptation", 9999) {}
    bool preventsAction() const override { return false; }
    bool impedeAcao() const override { return preventsAction(); }
    void applyTurnStart(Character* target) override { aplicarInicioTurno(target); }
    void aplicarInicioTurno(Character* target) override;
    void onExit(Character* target) override { aoSair(target); }
    void aoSair(Character* target) override;
    void adapt(Character* target, Character* enemy) { adaptar(target, enemy); }
    void adaptar(Character* target, Character* enemy);
};
