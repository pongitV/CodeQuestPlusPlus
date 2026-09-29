#pragma once
#include <utility>

class IAttacker {
public:
    virtual ~IAttacker() = default;
    
    // Calcula o dano base e perfurante gerado pelo atacante
    virtual std::pair<int, int> calculateBaseOffensiveDamage() = 0;
    
    // Garante um dano minimo caso possua propriedades ou buffs
    virtual int ensureMinimumDamage(int currentDamage) = 0;

    // Metodos legados para compatibilidade
    virtual std::pair<int, int> calcularDanoOfensivoBase() { return calculateBaseOffensiveDamage(); }
    virtual int garantirDanoMinimo(int danoAtual) { return ensureMinimumDamage(danoAtual); }
};
