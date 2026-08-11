#pragma once

#include "../ClassBase.h"
#include "../../races/RaceBase.h"
#include <string>
#include <vector>
#include <memory>

class Item;
class Combat;
class Character;

class RaceClone : public RaceBase {
private:
    std::string nomeOriginal;
    std::vector<std::string> aparenciaOriginal;
public:
    RaceClone(const std::string& n, const std::vector<std::string>& a);
    std::string getRaceName() const override;
    RaceType obterRaceType() const override;
    const std::vector<std::string>& getRaceAppearance() const override;
    Attributes getRaceAttributes() const override;
    std::string getRaceAbilityName() const override;
    std::string getRaceAbilityDescription() const override;
};

class PlayerClassClone : public ClassBase {
public:
    std::string getNameClasse() const override;
    ClassType obterClassType() const override;
    const std::vector<std::string>& obterAparenciaClasseMenu() const override;
    Attributes obterAtributosClasse() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override;
    std::string getNamePassivaClasse() const override;
    std::string obterDescricaoPassivaClasse() const override;
    std::string obterRecargaHabilidadeClasse() const override;
    std::string getNameHabilidadeClasse() const override;
    std::string getClassAbilityDescription() const override;
    void useClassAbility(Combat*, Character*, std::vector<Character*>&) override;
};
