#pragma once

#include <string>
#include <memory>

#include "BaseEquipment.h"
#include "../../../entities/character/Character.h"

class ArmorEquipment : public BaseEquipment 
{
private:
    std::string name;
    int fixedReduction;
    int reqResistance;
    int reqConstitution;
    int dexterityPenalty;

public:
    ArmorEquipment(const std::string& name, int fixedReduction, int reqResistance, int reqConstitution, int price = 3);
    
    int getReqResistance() const;
    int getReqConstitution() const;
    int obterReqResistencia() const { return getReqResistance(); }
    int obterReqConstituicao() const { return getReqConstitution(); }

    std::string getItemName() const override;
    EquipmentType getType() const override;

    int getFixedReduction() const override;
    int getFlatReduction() const { return getFixedReduction(); }
    void setDexterityPenalty(int penalty) { dexterityPenalty = penalty; }
    void definirPenalidadeDestreza(int pen) { setDexterityPenalty(pen); }

    std::string getStatusInfo() const override;

protected:
    bool checkSpecificRequirements(Character* character) const override;

public:
    bool canBeEquippedBy(Character* character) const override;
    bool isEquippable() const override { return true; }
    std::vector<std::string> getInspectionDetails(Character* character = nullptr) const override;

    std::unique_ptr<Item> generateUpgradedCopy() const override;
};

using EquipamentoArmadura = ArmorEquipment;

std::unique_ptr<Item> buildArmorEquipment(ItemID id);
inline std::unique_ptr<Item> fabricarEquipamentoArmadura(ItemID id) { return buildArmorEquipment(id); }
