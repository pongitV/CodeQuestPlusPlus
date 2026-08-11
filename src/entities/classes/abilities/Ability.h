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
    virtual std::string obterDescricao() const = 0;
    virtual int obterRecargaTurnos() const = 0;
    virtual bool consomeTurno() const { return true; }
    virtual void usar(Combat* combat, Character* usuario, std::vector<Character*>& inimigos) = 0;
};
