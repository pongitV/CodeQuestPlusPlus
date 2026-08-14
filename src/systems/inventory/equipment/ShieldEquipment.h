#pragma once

#include "BaseEquipment.h"
#include <string>
#include <memory>
#include "../../../entities/character/Character.h"

class ShieldEquipment : public BaseEquipment 
{
private:
    std::string name;
    int fixedReduction;
    int durability;
    int maxDurability;
    int reqResistance;
    int reqSecondary;
    AttributeType secondaryType;

public:
    ShieldEquipment(const std::string& name, int fixedReduction, int durability, int reqResistance, int reqSecondary, AttributeType secondaryType, int price = 3);
    
    int getReqResistance() const;
    int getReqSecondary() const;
    AttributeType getSecondaryType() const;

    int obterReqResistencia() const { return getReqResistance(); }
    int obterReqSecundario() const { return getReqSecondary(); }
    AttributeType obterTipoSecundario() const { return getSecondaryType(); }

    std::string getItemName() const override;
    EquipmentType getType() const override;

    int getShieldCurrentDurability() const override;
    int getCurrentShieldDurability() const { return getShieldCurrentDurability(); }
    int getMaxDurability() const;
    int obterDurabilidadeMaxima() const { return getMaxDurability(); }
    int getShieldFixedDamageReduction() const override;
    void setDurability(int newDurability);
    void definirDurabilidade(int novaDurabilidade) { setDurability(novaDurabilidade); }
    void reduceDurability(int qty) override;
    void increaseDurability(int qty) override;

    std::string getStatusInfo() const override;

protected:
    bool checkSpecificRequirements(Character* character) const override;

public:
    bool canBeEquippedBy(Character* character) const override;
    bool isEquippable() const override { return true; }
    std::vector<std::string> getInspectionDetails(Character* character = nullptr) const override;

    std::unique_ptr<Item> generateUpgradedCopy() const override;
};

using EquipamentoEscudo = ShieldEquipment;

std::unique_ptr<Item> buildShieldEquipment(ItemID id);
inline std::unique_ptr<Item> fabricarEquipamentoEscudo(ItemID id) { return buildShieldEquipment(id); }
