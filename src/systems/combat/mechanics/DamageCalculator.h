#pragma once
#include <utility>

class IAttacker;
class IDamageable;

// CalculadoraDeDano fornece funcoes estaticas para o calculo de dano em combate e mitigacao por armadura.
class CalculadoraDano {
public:
    // Calcula o dano base ofensivo e perfurante de um atacante
    static std::pair<int, int> calcularDanoOfensivoBase(IAttacker* atacante);
    
    // Calcula a mitigacao defensiva basica por armadura excluindo modificadores de parry
    static int calcularMitigacaoDefensiva(IDamageable* alvo, int danoBruto, int danoPerfurante);

    static std::pair<int, int> calculateOffensiveBaseDamage(IAttacker* attacker) {
        return calcularDanoOfensivoBase(attacker);
    }
    static int calculateDefensiveMitigation(IDamageable* target, int rawDamage, int piercingDamage) {
        return calcularMitigacaoDefensiva(target, rawDamage, piercingDamage);
    }
};

using CalculadoraDeDano = CalculadoraDano;
using DamageCalculator = CalculadoraDano;
