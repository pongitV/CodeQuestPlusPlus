#pragma once

#include "../Item.h"

/**
 * @brief Classe base padronizada para todos os equipamentos do jogo (Armas, Escudos e Armaduras).
 * Implementa o padrao Template Method para checagem de requisitos e inspecao de itens equipaveis.
 */
class BaseEquipment : public Item 
{
protected:
    virtual bool checkSpecificRequirements(Character* /*character*/) const { return true; }
    virtual bool checarRequisitosEspecificos(Character* character) const { return checkSpecificRequirements(character); }

public:
    BaseEquipment(int price = 3) : Item(price) {}
    virtual ~BaseEquipment() = default;

    bool isEquippable() const override { return true; }

    bool canBeEquippedBy(Character* character) const override {
        if (!character) return false;
        return checkSpecificRequirements(character);
    }

    std::vector<std::string> getBaseInspectionDetails(const std::string& typeName) const {
        std::vector<std::string> details;
        details.push_back(" > Tipo: " + typeName);
        return details;
    }
    std::vector<std::string> obterDetalhesInspecaoBase(const std::string& tipoNome) const {
        return getBaseInspectionDetails(tipoNome);
    }
};

using EquipamentoBase = BaseEquipment;
