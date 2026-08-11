#pragma once
#include <utility>

class IAttacker {
public:
    virtual ~IAttacker() = default;
    
    // Calcula o damage base e perfurante gerado pelo atacante
    virtual std::pair<int, int> calcularDanoOfensivoBase() = 0;
    
    // Garante um damage minimo caso possua propriedades ou buffs
    virtual int garantirDanoMinimo(int danoAtual) = 0;
};
