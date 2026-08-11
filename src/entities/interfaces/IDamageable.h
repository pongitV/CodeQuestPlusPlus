#pragma once
#include "../common/DamageResult.h"

class IAttacker;

class IDamageable {
public:
    virtual ~IDamageable() = default;
    
    virtual ResultadoDano receberDano(int danoBruto, int danoPerfurante, int danoReduzidoParry, IAttacker* atacante, bool aplicarPassivas = true) = 0;
    virtual int calcularDefesaBase(int danoBruto, int danoPerfurante) = 0;

    virtual ResultadoDano takeDamage(int rawDmg, int perfDmg, int parryRed, IAttacker* atk, bool applyPassives = true) {
        return receberDano(rawDmg, perfDmg, parryRed, atk, applyPassives);
    }
};
