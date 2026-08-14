#pragma once
#include "../common/DamageResult.h"

class IAttacker;

class IDamageable {
public:
    virtual ~IDamageable() = default;
    
    virtual DamageResult takeDamage(int rawDamage, int piercingDamage, int parryReducedDamage, IAttacker* attacker, bool applyPassives = true) = 0;
    virtual int calculateBaseDefense(int rawDamage, int piercingDamage) = 0;

    // Métodos legados para compatibilidade
    virtual DamageResult receberDano(int danoBruto, int danoPerfurante, int danoReduzidoParry, IAttacker* atacante, bool aplicarPassivas = true) {
        return takeDamage(danoBruto, danoPerfurante, danoReduzidoParry, atacante, aplicarPassivas);
    }
    virtual int calcularDefesaBase(int danoBruto, int danoPerfurante) {
        return calculateBaseDefense(danoBruto, danoPerfurante);
    }
};
