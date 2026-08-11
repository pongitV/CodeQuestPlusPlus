#pragma once

#include "BaseEquipment.h"
#include <string>
#include <set>
#include <memory>

class EquipamentoArma : public BaseEquipment 
{
private:
    std::string nome;
    int danoFisico;
    int danoMagico;
    int reqForca;
    int reqDestreza;
    int reqInteligencia;
    int reqSabedoria;
    bool efeitoSangramento;
    bool efeitoLentidao;

public:
    EquipamentoArma(const std::string& nome, int danoFisico, int danoMagico, int reqForca, int reqDestreza, int reqInteligencia, int reqSabedoria, int price = 3);
    
    int obterReqForca() const;
    int obterReqDestreza() const;
    int obterReqInteligencia() const;
    int obterReqSabedoria() const;

    std::string getNameItem() const override;
    void alterarNome(const std::string& n) override;
    TipoEquipamento obterTipo() const override;

    int obterDanoFisico() const override;
    int obterDanoMagico() const override;
    
    bool possuiEfeitoSangramento() const override;
    bool possuiEfeitoLentidao() const override;

    std::string obterInfoStatus() const override;

protected:
    bool checarRequisitosEspecificos(Character* character) const override;

public:
    bool podeSerEquipadoPor(Character* character) const override;
    bool isEquipavel() const override { return true; }
    std::vector<std::string> obterDetalhesInspecao(Character* character = nullptr) const override;

    void aplicarEfeitoSangramento() override;
    void aplicarEfeitoLentidao() override;
    
    void antesDeCausarDano(Character* atacante, Character* alvo) override;
    void aoCausarDano(Character* atacante, Character* alvo, int danoCausado) override;
    int garantirDanoMinimo(int finalDamage) override;

    std::unique_ptr<Item> gerarCopiaMelhorada() const override;
};

std::unique_ptr<Item> fabricarEquipamentoArma(ItemID id);
