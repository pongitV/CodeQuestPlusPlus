#pragma once

#include "../ClassBase.h"

class Item;
class Combat;

class Archer : public ClassBase 
{
public:
    // INFORMACOES DA CLASSE
    std::string getNameClasse() const override; 
    ClassType obterClassType() const override { return ClassType::Archer; } 
    std::string obterCaminhoSprite() const override { return "assets/classes/archer.png"; }
    const std::vector<std::string>& obterAparenciaClasseMenu() const override;
    Attributes obterAtributosClasse() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override;

    // PASSIVA DA CLASSE
    std::string getNamePassivaClasse() const override;
    std::string obterDescricaoPassivaClasse() const override;
    int processarPenalidadeArmaduraPassivaArqueiro(int penalidadeBase) const override;
    int aplicarPenalidadeLentidaoPassivaArqueiro(int dexterityAtual) const override;
    int reverterPenalidadeLentidaoPassivaArqueiro(int dexterityAtual) const override;

    // HABILIDADE DA CLASSE
    std::string obterRecargaHabilidadeClasse() const override;
    std::string getNameHabilidadeClasse() const override;
    std::string getClassAbilityDescription() const override;
    void useClassAbility(Combat* combat, Character* personagemUsuario, std::vector<Character*>& listaDeInimigos) override;
};
