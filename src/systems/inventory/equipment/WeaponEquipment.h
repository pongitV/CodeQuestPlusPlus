#pragma once

#include "BaseEquipment.h"
#include <string>
#include <set>
#include <memory>

class WeaponEquipment : public BaseEquipment 
{
private:
    std::string name;
    int physicalDamage;
    int magicalDamage;
    int reqStrength;
    int reqDexterity;
    int reqIntelligence;
    int reqWisdom;
    bool bleedEffect;
    bool slowEffect;

public:
    WeaponEquipment(const std::string& name, int physicalDamage, int magicalDamage, int reqStrength, int reqDexterity, int reqIntelligence, int reqWisdom, int price = 3);
    
    int getReqStrength() const;
    int getReqDexterity() const;
    int getReqIntelligence() const;
    int getReqWisdom() const;

    int obterReqForca() const { return getReqStrength(); }
    int obterReqDestreza() const { return getReqDexterity(); }
    int obterReqInteligencia() const { return getReqIntelligence(); }
    int obterReqSabedoria() const { return getReqWisdom(); }

    std::string getItemName() const override;
    void changeName(const std::string& n) override;
    EquipmentType getType() const override;

    int getPhysicalDamage() const override;
    int getMagicalDamage() const override;
    
    bool hasBleedEffect() const override;
    bool hasSlowEffect() const override;

    std::string getStatusInfo() const override;

protected:
    bool checkSpecificRequirements(Character* character) const override;

public:
    bool canBeEquippedBy(Character* character) const override;
    bool isEquippable() const override { return true; }
    std::vector<std::string> getInspectionDetails(Character* character = nullptr) const override;

    void applyBleedEffect() override;
    void applySlowEffect() override;
    
    void beforeDealingDamage(Character* attacker, Character* target) override;
    void onDealingDamage(Character* attacker, Character* target, int damageDealt) override;
    int ensureMinimumDamage(int finalDamage) override;

    std::unique_ptr<Item> generateUpgradedCopy() const override;
};

using EquipamentoArma = WeaponEquipment;

std::unique_ptr<Item> buildWeaponEquipment(ItemID id);
inline std::unique_ptr<Item> fabricarEquipamentoArma(ItemID id) { return buildWeaponEquipment(id); }
