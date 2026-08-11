#pragma once

#include "../ClassBase.h"

class Item;
class Combat;

class Mage : public ClassBase
{
private:
    TipoAtaque tipoAtaqueAtual = TipoAtaque::UNICO;

public:
    // INFORMACOES DA CLASSE
    std::string getNameClasse() const override;
    ClassType obterClassType() const override { return ClassType::Mage; }
    std::string obterCaminhoSprite() const override { return "assets/classes/mage.png"; }
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
    void processarDanoPosAtaque(Character* atacante, Character* alvoAtual, Character* defensorPrincipal, int baseDamage, int danoPerfurante, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool isAtacanteJogador, bool isArea, bool& ativouPassiva) override;
};
