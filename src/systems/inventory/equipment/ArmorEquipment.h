#pragma once

#include <string>
#include <memory>

#include "BaseEquipment.h"
#include "../../../entities/character/Character.h"

class EquipamentoArmadura : public BaseEquipment 
{
private:
    std::string nome;
    int reducaoFixa;
    int reqResistencia;
    int reqConstituicao;
    int penalidadeDestreza;

public:
    EquipamentoArmadura(const std::string& nome, int reducaoFixa, int reqResistencia, int reqConstituicao, int price = 3);
    
    int obterReqResistencia() const;
    int obterReqConstituicao() const;

    std::string getNameItem() const override;
    TipoEquipamento obterTipo() const override;

    int obterReducaoFixa() const override;
    void definirPenalidadeDestreza(int pen) { penalidadeDestreza = pen; }

    std::string obterInfoStatus() const override;

protected:
    bool checarRequisitosEspecificos(Character* character) const override;

public:
    bool podeSerEquipadoPor(Character* character) const override;
    bool isEquipavel() const override { return true; }
    std::vector<std::string> obterDetalhesInspecao(Character* character = nullptr) const override;

    std::unique_ptr<Item> gerarCopiaMelhorada() const override;
};

std::unique_ptr<Item> fabricarEquipamentoArmadura(ItemID id);
