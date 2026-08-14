#include "Status.h"

#include <iostream>

#include "../../entities/classes/ClassBase.h"
#include "../../entities/character/Character.h"
#include "../utils/DialogFunctions.h"
#include "../../ui/screens/combat/ScreenCombat.h"
#include "../../core/utils/Color.h"

void LifeStealEffect::applyTurnStart(Character* target) {
    if (!Character::isValid(attacker) || attacker->getHealth() <= 0) return;
    int rootDamage = target->getHealth() / 5;
    if (rootDamage > 0) 
    {
        target->modifyHealth(-rootDamage);
        attacker->modifyHealth(rootDamage);
        TelaCombate::adicionarMensagemFixa(target->getName() + " sofreu " + std::to_string(rootDamage) + " de dano de Suga Sangue!");
    }
}

void NecrosisEffect::applyTurnStart(Character* target) {
    if (target->getHealth() <= 0) return;
    target->modifyHealth(-damagePerTurn);
    TelaCombate::adicionarMensagemFixa(target->getName() + " sofreu " + std::to_string(damagePerTurn) + " de dano de Necrose!");
}

void SlowEffect::onEnter(Character* target) {
    if (target->getClass()) target->getFinalStats().dexterity = target->getClass()->applyArcherPassiveSlowPenalty(target->getFinalStats().dexterity);
    else target->getFinalStats().dexterity /= 2;
}

void SlowEffect::onExit(Character* target) {
    if (target->getClass()) target->getFinalStats().dexterity = target->getClass()->revertArcherPassiveSlowPenalty(target->getFinalStats().dexterity);
    else target->getFinalStats().dexterity *= 2;
}

void WeaknessEffect::onEnter(Character* target) {
    lostStrength = target->getFinalStats().strength / 4;
    target->getFinalStats().strength -= lostStrength;
}

void WeaknessEffect::onExit(Character* target) {
    target->getFinalStats().strength += lostStrength;
}

void ArmorBreakEffect::onEnter(Character* target) {
    lostResistance = static_cast<int>(target->getFinalStats().resistance * 0.20);
    lostConstitution = static_cast<int>(target->getFinalStats().constitution * 0.10);
    target->getFinalStats().resistance -= lostResistance;
    target->getFinalStats().constitution -= lostConstitution;
}

void ArmorBreakEffect::onExit(Character* target) {
    target->getFinalStats().resistance += lostResistance;
    target->getFinalStats().constitution += lostConstitution;
}

void ArmorBreakEffect::applyTurnStart(Character* /*target*/) {
}

void BleedingEffect::applyTurnStart(Character* target) {
    if (target->getHealth() <= 0) return;
    target->modifyHealth(-damagePerTurn);
    TelaCombate::adicionarMensagemFixa(target->getName() + " sofreu " + std::to_string(damagePerTurn) + " de dano por Sangramento!");
}

int HalfDamageEffect::processIncomingDamage(int damage) {
    int reducedDamage = damage / 2;
    return reducedDamage;
}

void WarCryEffect::onEnter(Character* target) {
    target->getFinalStats().strength += strengthBonus;
    target->getFinalStats().dexterity += dexterityBonus;
}

void WarCryEffect::onExit(Character* target) {
    target->getFinalStats().strength -= strengthBonus;
    target->getFinalStats().dexterity -= dexterityBonus;
}

void AttributeBuffEffect::onExit(Character* target) {
    if (target->getMultiplier() != 1.0) {
        target->setMultiplier(1.0);
    }
}

void AdaptationWheelEffect::applyTurnStart(Character* target) {
    if (target->getHealth() <= 0) return;
    if (!target->getArmor() || !target->getArmor()->hasProperty(Property::AdaptationArmor)) return;
    
    int healAmount = target->getMaxHealth() * 0.05;
    if (healAmount > 0) {
        std::string msg;
        msg.reserve(64);
        msg = TelaCombate::margemCombate();
        msg += ">> A Roda gira... Regenerou ";
        msg += std::to_string(healAmount);
        msg += " HP!\n";
        TelaCombate::adicionarMensagemFixa(msg);
    }
}

void AdaptationWheelEffect::onExit(Character* target) {
    target->getFinalStats().strength -= bonusStrength;
    target->getFinalStats().dexterity -= bonusDexterity;
    target->getFinalStats().resistance -= bonusResistance;
    target->getFinalStats().constitution -= bonusConstitution;
    target->getFinalStats().intelligence -= bonusIntelligence;
    target->getFinalStats().wisdom -= bonusWisdom;
    target->forceCacheRecalculation();
}

void AdaptationWheelEffect::adapt(Character* target, Character* enemy) {
    if (!enemy) return;
    
    // --- 1. Adaptação Defensiva (Baseada no inimigo) ---
    int enemyPhysicalPower = enemy->getStrength() + enemy->getDexterity();
    int enemyMagicalPower = enemy->getIntelligence() + enemy->getWisdom();
    
    std::string msgDefesa;
    if (enemyPhysicalPower >= enemyMagicalPower) {
        target->alterStaticAttribute(AttributeType::Resistance, 2); 
        target->alterStaticAttribute(AttributeType::Constitution, 2);
        bonusResistance += 2; bonusConstitution += 2;
        msgDefesa = "defesa fisica";
    } else {
        target->alterStaticAttribute(AttributeType::Wisdom, 2); 
        target->alterStaticAttribute(AttributeType::Constitution, 2);
        bonusWisdom += 2; bonusConstitution += 2;
        msgDefesa = "defesa magica";
    }

    // --- 2. Adaptação Ofensiva (Baseada na arma do jogador) ---
    int weaponPhysicalDamage = 1;
    int weaponMagicalDamage = 0;
    if (target->getWeapon()) {
        weaponPhysicalDamage = target->getWeapon()->obterDanoFisico();
        weaponMagicalDamage = target->getWeapon()->obterDanoMagico();
    }

    std::string msgAtaque;
    if (weaponMagicalDamage > weaponPhysicalDamage) {
        target->alterStaticAttribute(AttributeType::Intelligence, 2); 
        target->alterStaticAttribute(AttributeType::Wisdom, 2);
        bonusIntelligence += 2; bonusWisdom += 2;
        msgAtaque = "poder magico";
    } else if (weaponPhysicalDamage > weaponMagicalDamage) {
        target->alterStaticAttribute(AttributeType::Strength, 2); 
        target->alterStaticAttribute(AttributeType::Dexterity, 2);
        bonusStrength += 2; bonusDexterity += 2;
        msgAtaque = "poder fisico";
    } else {
        // Armas híbridas (ex: Espada de Extermínio)
        target->alterStaticAttribute(AttributeType::Strength, 2); 
        target->alterStaticAttribute(AttributeType::Dexterity, 2);
        target->alterStaticAttribute(AttributeType::Intelligence, 2);
        bonusStrength += 2; bonusDexterity += 2; bonusIntelligence += 2;
        msgAtaque = "poder hibrido";
    }

    TelaCombate::adicionarMensagemFixa(TelaCombate::margemCombate() + "* KLINK! * A Roda adapta " + msgDefesa + " e " + msgAtaque + " (+2)!\n");
}

void InviolableEffect::onExit(Character* target) { target->addEffect(std::make_unique<TrueAimEffect>(99)); }
