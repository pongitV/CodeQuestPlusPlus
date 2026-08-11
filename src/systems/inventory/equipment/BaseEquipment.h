#pragma once

#include "../Item.h"

/**
 * @brief Classe base padronizada para todos os equipamentos do jogo (Armas, Escudos e Armaduras).
 * Implementa o padrao Template Method para checagem de requisitos e inspecao de itens equipaveis.
 */
class BaseEquipment : public Item 
{
protected:
    virtual bool checarRequisitosEspecificos(Character* /*character*/) const { return true; }

public:
    BaseEquipment(int price = 3) : Item(price) {}
    virtual ~BaseEquipment() = default;

    bool isEquipavel() const override { return true; }

    bool podeSerEquipadoPor(Character* character) const override {
        if (!character) return false;
        return checarRequisitosEspecificos(character);
    }

    std::vector<std::string> obterDetalhesInspecaoBase(const std::string& tipoNome) const {
        std::vector<std::string> detalhes;
        detalhes.push_back(" > Tipo: " + tipoNome);
        return detalhes;
    }
};
