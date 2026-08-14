#include "WeaponEquipment.h"

#include <iostream>
#include <vector>

#include "../../../entities/character/Character.h"
#include "../../../core/utils/RandomGenerator.h"
#include "../../../ui/screens/combat/ScreenCombat.h"
#include <functional>
#include <unordered_map>
#include "../ItemFactory.h"
#include "../../../core/utils/Color.h"

bool Item::checkAttributeRequirements(Character* character, int reqStrength, int reqDexterity, int reqIntelligence, int reqWisdom) {
    if (!character) return false;
    return character->getStrength() >= reqStrength &&
           character->getDexterity() >= reqDexterity &&
           character->getIntelligence() >= reqIntelligence &&
           character->getWisdom() >= reqWisdom;
}

WeaponEquipment::WeaponEquipment(const std::string& name, int physicalDamage, int magicalDamage, int reqStrength, int reqDexterity, int reqIntelligence, int reqWisdom, int price)
    : BaseEquipment(price), name(name), physicalDamage(physicalDamage), magicalDamage(magicalDamage), reqStrength(reqStrength), reqDexterity(reqDexterity), reqIntelligence(reqIntelligence), reqWisdom(reqWisdom), bleedEffect(false), slowEffect(false)
{
}

std::string WeaponEquipment::getItemName() const { return name; }
void WeaponEquipment::changeName(const std::string& n) { name = n; }
EquipmentType WeaponEquipment::getType() const { return EquipmentType::Weapon; }

int WeaponEquipment::getPhysicalDamage() const { return physicalDamage; }
int WeaponEquipment::getMagicalDamage() const { return magicalDamage; }

int WeaponEquipment::getReqStrength() const { return reqStrength; }
int WeaponEquipment::getReqDexterity() const { return reqDexterity; }
int WeaponEquipment::getReqIntelligence() const { return reqIntelligence; }
int WeaponEquipment::getReqWisdom() const { return reqWisdom; }

bool WeaponEquipment::hasBleedEffect() const { return bleedEffect; }
bool WeaponEquipment::hasSlowEffect() const { return slowEffect; }

bool WeaponEquipment::checkSpecificRequirements(Character* character) const {
    return Item::checkAttributeRequirements(character, reqStrength, reqDexterity, reqIntelligence, reqWisdom);
}

bool WeaponEquipment::canBeEquippedBy(Character* character) const {
    return BaseEquipment::canBeEquippedBy(character);
}

std::vector<std::string> WeaponEquipment::getInspectionDetails(Character* character) const {
    std::vector<std::string> lines;
    lines.push_back(" > Tipo: Weapon");

    std::string fisStr = std::to_string(physicalDamage);
    std::string magStr = std::to_string(magicalDamage);

    if (character) {
        int strength = character->getStrength();
        int dexterity = character->getDexterity();
        int inteli = character->getIntelligence();
        int wisdom = character->getWisdom();
        
        if (physicalDamage == 0 && magicalDamage > 0) { strength /= 10; dexterity /= 10; }
        else if (physicalDamage > 0 && magicalDamage == 0) { inteli /= 10; wisdom /= 10; }
        
        int danoFisEst = std::max(0, static_cast<int>((physicalDamage + strength) * (1.0 + (dexterity / 100.0)) * character->getMultiplier()));
        int danoMagEst = std::max(0, static_cast<int>((magicalDamage + inteli) * (1.0 + (wisdom / 100.0)) * character->getMultiplier()));
        
        fisStr += " -> C/ Seus Attributes: " + std::to_string(danoFisEst);
        magStr += " -> C/ Seus Attributes: " + std::to_string(danoMagEst);
    }

    lines.push_back(" > Damage Fisico: " + fisStr);
    lines.push_back(" > Damage Magico: " + magStr);
    lines.push_back(" > Requisitos:");
    bool hasReq = false;
    if (reqStrength > 0) { lines.push_back("   - Forca: " + std::to_string(reqStrength)); hasReq = true; }
    if (reqDexterity > 0) { lines.push_back("   - Destreza: " + std::to_string(reqDexterity)); hasReq = true; }
    if (reqIntelligence > 0) { lines.push_back("   - Inteligencia: " + std::to_string(reqIntelligence)); hasReq = true; }
    if (reqWisdom > 0) { lines.push_back("   - Sabedoria: " + std::to_string(reqWisdom)); hasReq = true; }
    if (!hasReq) lines.push_back("   - Nenhum requisito.");
    
    lines.push_back(" > Efeitos e Propriedades:");
    bool hasEfeito = false;
    if (bleedEffect) { lines.push_back("   - Sangramento (Damage continuo no alvo)"); hasEfeito = true; }
    if (slowEffect) { lines.push_back("   - Lentidao (Reduz dexterity do alvo)"); hasEfeito = true; }
    if (hasProperty(Property::Piercing)) { lines.push_back("   - Penetrante (Reduz resistance do alvo)"); hasEfeito = true; }
    if (hasProperty(Property::Magic)) { lines.push_back("   - Magica (Parte do damage ignora defesa)"); hasEfeito = true; }
    if (hasProperty(Property::IgnoreDefense)) { lines.push_back("   - Exterminio (Ignora 100% da Resistencia e Constituicao do alvo)"); hasEfeito = true; }
    if (hasProperty(Property::MagicGuitar)) { lines.push_back("   - Raizes Drenantes (Causa damage e cura o usuario)"); hasEfeito = true; }
    if (hasProperty(Property::VineTrap)) { lines.push_back("   - Prisao de Cipos (Chance de atordoar alvo)"); hasEfeito = true; }
    if (!hasEfeito) lines.push_back("   - Nenhuma propriedade extra.");
    return lines;
}

std::string WeaponEquipment::getStatusInfo() const {
    std::string ef = "";
    if (hasBleedEffect()) ef += " | +Sangramento";
    if (hasSlowEffect()) ef += " | +Lentidao";
    if (hasProperty(Property::Piercing)) ef += " | +Penetracao";
    
    std::string reqs = "";
    bool hasReq = false;
    if (reqStrength > 0) { reqs += std::to_string(reqStrength) + " For "; hasReq = true; }
    if (reqDexterity > 0) { reqs += std::to_string(reqDexterity) + " Des "; hasReq = true; }
    if (reqIntelligence > 0) { reqs += std::to_string(reqIntelligence) + " Int "; hasReq = true; }
    if (reqWisdom > 0) { reqs += std::to_string(reqWisdom) + " Sab "; hasReq = true; }
    if (hasReq) reqs = " | Req: " + reqs;

    return " (Damage: " + std::to_string(physicalDamage) + "F/" + std::to_string(magicalDamage) + "M" + ef + reqs + ")";
}

void WeaponEquipment::applyBleedEffect() { bleedEffect = true; }
void WeaponEquipment::applySlowEffect() { slowEffect = true; }

void WeaponEquipment::beforeDealingDamage(Character* attacker, Character* target) {
    if (hasProperty(Property::Piercing) && !target->hasEffect(EffectID::ArmorBreak)) {
        target->addEffect(std::make_unique<ArmorBreakEffect>());
        TelaCombate::adicionarMensagemFixa(TelaCombate::margemCombate() + ">> A arma de " + attacker->getName() + " ativou o po magico! O ataque enfraqueceu " + target->getName() + " ate o fim do combat!\n");
    }
}

void WeaponEquipment::onDealingDamage(Character* attacker, Character* target, int damageDealt) {
    if (damageDealt <= 0) return;

    if (hasProperty(Property::MagicGuitar) && !target->hasEffect(EffectID::LifeSteal)) {
        target->addEffect(std::make_unique<LifeStealEffect>(2, attacker));
    }

    if (hasProperty(Property::VineTrap) && RandomGenerator::rolarChance(30) && !target->hasEffect(EffectID::Stun)) {
        target->addEffect(std::make_unique<StunEffect>(1));
    }

    if (hasBleedEffect() && !target->hasEffect(EffectID::Bleeding)) {
        int bleedDmg = std::max(1, target->getMaxHealth() / 10);
        target->addEffect(std::make_unique<BleedingEffect>(3, bleedDmg));
        Color bleedColor = (target->getClassName() != "Monstro") ? Color::LIGHT_RED : Color::RED;
        TelaCombate::adicionarMensagemFixa(TelaCombate::margemCombate() + ">> " + target->getName() + " comecou a sangrar profundamente! (3 turnos)\n");
    }

    if (hasSlowEffect() && !target->hasEffect(EffectID::Slow)) {
        target->addEffect(std::make_unique<SlowEffect>(3));
        TelaCombate::adicionarMensagemFixa(TelaCombate::margemCombate() + ">> " + target->getName() + " foi coberto por gosma e sua dexterity caiu pela metade! (3 turnos)\n");
    }
}

int WeaponEquipment::ensureMinimumDamage(int finalDamage) {
    int minDamage = 1;
    if (hasProperty(Property::BasicGuitar)) {
        minDamage = std::max(minDamage, magicalDamage);
    }
    return std::max(finalDamage, minDamage);
}

std::unique_ptr<Item> WeaponEquipment::generateUpgradedCopy() const {
    auto newWeapon = std::make_unique<WeaponEquipment>(name + "+", static_cast<int>(physicalDamage * 1.5), static_cast<int>(magicalDamage * 1.5), reqStrength, reqDexterity, reqIntelligence, reqWisdom, sellPrice * 2);
    
    for (Property prop : properties) newWeapon->addProperty(prop);
    newWeapon->addProperty(Property::Upgraded);
    
    if (bleedEffect) newWeapon->applyBleedEffect();
    if (slowEffect) newWeapon->applySlowEffect();
    
    return newWeapon;
}

std::unique_ptr<Item> buildWeaponEquipment(ItemID id) {
    auto createWeapon = [](ItemID id, int dPhys, int dMag, int rStr, int rDex, int rInt, int rWis, int price) {
        return std::make_unique<WeaponEquipment>(ItemFactory::getNameDeID(id), dPhys, dMag, rStr, rDex, rInt, rWis, price);
    };

    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> constructors = {
        {ItemID::StoneDagger, [createWeapon]() { return createWeapon(ItemID::StoneDagger, 5, 0, 0, 0, 0, 0, 3); }},
        {ItemID::WoodBow, [createWeapon]() { return createWeapon(ItemID::WoodBow, 10, 0, 0, 0, 0, 0, 3); }},
        {ItemID::CrystalStaff, [createWeapon]() { return createWeapon(ItemID::CrystalStaff, 0, 30, 0, 0, 0, 0, 3); }},
        {ItemID::CorrodedWand, [createWeapon]() { return createWeapon(ItemID::CorrodedWand, 0, 25, 0, 0, 0, 0, 3); }},
        {ItemID::EnchantedGuitar, [createWeapon]() { 
            auto guitar = createWeapon(ItemID::EnchantedGuitar, 0, 10, 0, 0, 0, 0, 3); 
            guitar->addProperty(Property::BasicGuitar);
            return guitar; 
        }},
        {ItemID::BoneStaff, [createWeapon]() { 
            auto weapon = createWeapon(ItemID::BoneStaff, 2, 8, 0, 0, 5, 10, 10); 
            weapon->addProperty(Property::Magic);
            return weapon; 
        }},
        {ItemID::IronSword, [createWeapon]() { return createWeapon(ItemID::IronSword, 10, 0, 0, 0, 0, 0, 3); }},
        {ItemID::BattleAxe, [createWeapon]() { return createWeapon(ItemID::BattleAxe, 15, 0, 10, 0, 0, 0, 3); }},
        {ItemID::AcidSlimeWeapon, [createWeapon]() { return createWeapon(ItemID::AcidSlimeWeapon, 2, 7, 0, 0, 0, 0, 3); }},
        {ItemID::CrumpledTrunk, [createWeapon]() { return createWeapon(ItemID::CrumpledTrunk, 40, 0, 25, 0, 0, 0, 30); }},
        {ItemID::KnightSword, [createWeapon]() { return createWeapon(ItemID::KnightSword, 12, 0, 0, 0, 0, 0, 0); }},
        {ItemID::ExterminationSword, []() { 
            std::string name = "Espada de Exterminio";
            auto weapon = std::make_unique<WeaponEquipment>(name, 65, 65, 40, 40, 0, 0, 5000);
            weapon->addProperty(Property::Magic);
            weapon->addProperty(Property::IgnoreDefense);
            return weapon;
        }}
    };
    auto it = constructors.find(id);
    if (it != constructors.end()) return it->second();
    return nullptr;
}
