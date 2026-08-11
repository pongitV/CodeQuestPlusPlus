#pragma once

#include "../ClassBase.h"

class Item;
class Combat;

class Warrior : public ClassBase
{
public:
    // INFORMACOES DA CLASSE
    std::string getNameClasse() const override; 
    ClassType obterClassType() const override { return ClassType::Warrior; } 
    std::string obterCaminhoSprite() const override { return "assets/classes/knight.png"; }
    const std::vector<std::string>& obterAparenciaClasseMenu() const override;
    Attributes obterAtributosClasse() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override;

    // PASSIVA DA CLASSE
    std::string getNamePassivaClasse() const override;
    std::string obterDescricaoPassivaClasse() const override;

    // HABILIDADE DA CLASSE
    std::string obterRecargaHabilidadeClasse() const override;
    std::string getNameHabilidadeClasse() const override;
    std::string getClassAbilityDescription() const override;
    void useClassAbility(Combat* combat, Character* personagemUsuario, std::vector<Character*>& listaDeInimigos) override;

protected:
    // PROCESSAMENTO DE DANO 
    int processarDanoPreAtaque(Character* atacante, Character* defensor, int baseDamage, bool isAtacanteJogador, size_t qtdInimigos) override;
};  
