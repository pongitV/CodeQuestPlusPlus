#pragma once

#include <string>
#include <vector>
#include <memory>

class Character;
class Combat;

class Ability {
public:
    virtual ~Ability() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual int getCooldownTurns() const = 0;
    virtual bool consumesTurn() const { return true; }
    virtual void use(Combat* combat, Character* user, std::vector<Character*>& enemies) = 0;

    // Metodos legados para compatibilidade
    virtual std::string obterDescricao() const { return getDescription(); }
    virtual int obterRecargaTurnos() const { return getCooldownTurns(); }
    virtual bool consomeTurno() const { return consumesTurn(); }
    virtual void usar(Combat* combat, Character* usuario, std::vector<Character*>& inimigos) { use(combat, usuario, inimigos); }
};

using Habilidade = Ability;
