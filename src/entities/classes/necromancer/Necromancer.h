#pragma once

#include "../ClassBase.h"

class Necromancer : public ClassBase {
public:
    // --- INFORMACOES DA CLASSE ---
    std::string getNameClasse() const override;
    ClassType obterClassType() const override { return ClassType::NECROMANTE; }
    std::string obterCaminhoSprite() const override { return "assets/classes/necromancer.png"; }
    const std::vector<std::string>& obterAparenciaClasseMenu() const override;
    Attributes obterAtributosClasse() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override;

    // --- HABILIDADE DA CLASSE ---
    std::string getNameHabilidadeClasse() const override;
    std::string getClassAbilityDescription() const override;
    std::string obterRecargaHabilidadeClasse() const override;
    void useClassAbility(Combat* combat, Character* personagemUsuario, std::vector<Character*>& listaDeInimigos) override;

    // --- PASSIVA DA CLASSE ---
    std::string getNamePassivaClasse() const override;
    std::string obterDescricaoPassivaClasse() const override;
    void executarAtaqueComPassivaDaClasse(Character* atacante, Character* defensor, int baseDamage, int danoPerfurante, std::vector<std::unique_ptr<Character>>& listaDeInimigos, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool aplicarPassiva) override;
};
