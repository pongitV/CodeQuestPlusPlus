#pragma once

#include "BaseEquipment.h"
#include <string>
#include <memory>
#include "../../../entities/character/Character.h"


class EquipamentoEscudo : public BaseEquipment 
{
private:
    std::string nome;
    int reducaoFixa;
    int durabilidade;
    int durabilidadeMaxima;
    int reqResistencia;
    int reqSecundario;
    TipoAtributo tipoSecundario;

public:
    EquipamentoEscudo(const std::string& nome, int reducaoFixa, int durabilidade, int reqResistencia, int reqSecundario, TipoAtributo tipoSecundario, int price = 3);
    
    int obterReqResistencia() const;
    int obterReqSecundario() const;
    TipoAtributo obterTipoSecundario() const;

    std::string getNameItem() const override;
    TipoEquipamento obterTipo() const override;

    int obterDurabilidadeAtualEscudo() const override;
    int obterDurabilidadeMaxima() const;
    int obterReducaoDanoFixaEscudo() const override;
    void definirDurabilidade(int novaDurabilidade);
    void reduzirDurabilidade(int qtd) override;
    void aumentarDurabilidade(int qtd) override;

    std::string obterInfoStatus() const override;

protected:
    bool checarRequisitosEspecificos(Character* character) const override;

public:
    bool podeSerEquipadoPor(Character* character) const override;
    bool isEquipavel() const override { return true; }
    std::vector<std::string> obterDetalhesInspecao(Character* character = nullptr) const override;

    std::unique_ptr<Item> gerarCopiaMelhorada() const override;
};

std::unique_ptr<Item> fabricarEquipamentoEscudo(ItemID id);
