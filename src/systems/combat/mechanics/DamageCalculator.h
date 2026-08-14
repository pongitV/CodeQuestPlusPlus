#pragma once
#include <utility>

class IAttacker;
class IDamageable;

// Calculadora de dano em combate e mitigacao por armadura.
class DamageCalculator {
public:
    // Calcula o dano base ofensivo e perfurante de um atacante
    static std::pair<int, int> calculateOffensiveBaseDamage(IAttacker* attacker);
    
    // Calcula a mitigacao defensiva basica por armadura excluindo modificadores de parry
    static int calculateDefensiveMitigation(IDamageable* target, int rawDamage, int piercingDamage);

    // Compatibilidade legada
    static std::pair<int, int> calcularDanoOfensivoBase(IAttacker* atacante) {
        return calculateOffensiveBaseDamage(atacante);
    }
    static int calcularMitigacaoDefensiva(IDamageable* alvo, int danoBruto, int danoPerfurante) {
        return calculateDefensiveMitigation(alvo, danoBruto, danoPerfurante);
    }
};

using CalculadoraDano = DamageCalculator;
using CalculadoraDeDano = DamageCalculator;
