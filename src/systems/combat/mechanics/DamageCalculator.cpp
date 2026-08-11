#include "DamageCalculator.h"
#include "../../../entities/interfaces/IAttacker.h"
#include "../../../entities/interfaces/IDamageable.h"

std::pair<int, int> CalculadoraDano::calcularDanoOfensivoBase(IAttacker* atacante) {
    if (atacante) {
        return atacante->calcularDanoOfensivoBase();
    }
    return {0, 0};
}

int CalculadoraDano::calcularMitigacaoDefensiva(IDamageable* alvo, int danoBruto, int danoPerfurante) {
    if (alvo) {
        return alvo->calcularDefesaBase(danoBruto, danoPerfurante);
    }
    return danoBruto + danoPerfurante;
}
