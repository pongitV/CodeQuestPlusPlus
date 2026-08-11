#pragma once

#include <string>
#include <vector>

#include "../classes/ClassBase.h"

class ClassBaseInimigo : public ClassBase
{
public:
    std::string getNameClasse() const override;
    ClassType obterClassType() const override { return ClassType::Nenhum; }
    Attributes obterAtributosClasse() const override;
    const std::vector<std::string>& obterAparenciaClasseMenu() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override;

    std::string getNamePassivaClasse() const override;
    std::string obterDescricaoPassivaClasse() const override;
    std::string obterRecargaHabilidadeClasse() const override;

    void useClassAbility(Combat* combat, Character* personagemUsuario, std::vector<Character*>& listaDeInimigos) override;
    std::string getNameHabilidadeClasse() const override;
    std::string getClassAbilityDescription() const override;
    
    TipoAtaque getAttackType() const override;
    bool abilityConsumesTurn() const override;
};
