#include "ConsumableItem.h"
#include "../../../core/state/Status.h"
#include "../../../entities/character/Character.h"
#include "../../../ui/screens/combat/ScreenCombat.h"
#include "../../../ui/UIManager.h"
#include "../../../core/utils/InputControl.h"
#include <functional>
#include <unordered_map>
#include "../ItemFactory.h"
#include "../../../core/utils/Color.h"

static void showConsumableNotice(const std::string& msg, Color colorMsg = Color::WHITE) {
    (void)colorMsg;
    TelaCombate::adicionarMensagemFixa(msg);
}

ConsumableItem::ConsumableItem(const std::string& name, int price) : Item(price), name(name)
{
}

std::string ConsumableItem::getItemName() const { return name; }

EquipmentType ConsumableItem::getType() const { return EquipmentType::Consumable; }

std::vector<std::string> ConsumableItem::getInspectionDetails(Character* /*personagem*/) const {
    std::vector<std::string> details;
    details.push_back(" > Tipo: Consumable");
    if (!inspectionDescription.empty()) {
        for (const auto& desc : inspectionDescription) details.push_back(" > Efeitos: " + desc);
    } else {
        details.push_back(" > Efeitos: Pode ser consumido para aplicar efeitos.");
    }
    return details;
}

std::unique_ptr<Item> buildConsumableItem(ItemID id) {
    auto createHealPotion = []() {
        auto heal = std::make_unique<ConsumableItem>(ItemFactory::getNameDeID(ItemID::HealPotion30), 6);
        heal->addProperty(Property::HealingConsumable);
        heal->setInspectionDescription("Restaura 30% da sua Health Maxima.");
        heal->setInventoryAction([](Item* item, Character* user, bool* turnConsumed) {
            if (user->getHealth() >= user->getMaxHealth()) {
                showConsumableNotice("Sua health ja esta cheia!");
                return true;
            }
            int healthBefore = user->getHealth();
            int estimatedHeal = static_cast<int>(user->getMaxHealth() * 0.30);
            user->modifyHealth(estimatedHeal);
            int healthAfter = user->getHealth();
            int actualHeal = healthAfter - healthBefore;
            showConsumableNotice(item->getItemName() + " usada! +" + std::to_string(actualHeal) + " HP. (Health atual: " + std::to_string(healthAfter) + "/" + std::to_string(user->getMaxHealth()) + ")", Color::GREEN);
            
            if (user->getQuickConsumable() == item) {
                user->unequipConsumable();
                std::string thisItemName = item->getItemName();
                for (auto* otherItem : user->getInventory()->obterTodosOsItens()) {
                    if (otherItem != item && otherItem->getItemName() == thisItemName) {
                        user->equipItem(otherItem);
                        break;
                    }
                }
            }

            user->getInventory()->removerItem(item);
            if (turnConsumed) *turnConsumed = true;
            return true;
        });
        return heal;
    };

    auto createTalisman = [](ItemID id, Property prop, AttributeType buffAttr, AttributeType debuffAttr, const std::string& desc) {
        auto t = std::make_unique<ConsumableItem>(ItemFactory::getNameDeID(id), 120);
        t->addProperty(prop);
        t->setInspectionDescription(desc);
        t->setInventoryAction([buffAttr, debuffAttr](Item* item, Character* user, bool* turnConsumed) {
            user->modifyStaticAttribute(buffAttr, 5);
            user->modifyStaticAttribute(debuffAttr, -5);
            showConsumableNotice(item->getItemName() + " consumido!");
            
            if (user->getQuickConsumable() == item) {
                user->unequipConsumable();
                std::string thisItemName = item->getItemName();
                for (auto* otherItem : user->getInventory()->obterTodosOsItens()) {
                    if (otherItem != item && otherItem->getItemName() == thisItemName) {
                        user->equipItem(otherItem);
                        break;
                    }
                }
            }

            user->getInventory()->removerItem(item);
            if (turnConsumed) *turnConsumed = true;
            return true;
        });
        return t;
    };

    auto createAttributeBuff = [](ItemID id) {
        auto buff = std::make_unique<ConsumableItem>(ItemFactory::getNameDeID(id), 3);
        buff->addProperty(Property::BuffConsumable);
        buff->setInspectionDescription("Aumenta seus attributes em 1.5x por 2 turnos.");
        buff->setInventoryAction([](Item* item, Character* user, bool* turnConsumed) {
            if (!turnConsumed) { showConsumableNotice("Pocoes de buff so podem ser usadas em combat!"); return true; }
            user->addEffect(std::make_unique<AttributeBuffEffect>(2));
            user->setMultiplier(1.5);
            showConsumableNotice(item->getItemName() + " consumida! Attributes ampliados em 1.5x por 2 turnos!", Color::LIGHT_GREEN);
            
            if (user->getQuickConsumable() == item) {
                user->unequipConsumable();
                std::string thisItemName = item->getItemName();
                for (auto* otherItem : user->getInventory()->obterTodosOsItens()) {
                    if (otherItem != item && otherItem->getItemName() == thisItemName) {
                        user->equipItem(otherItem);
                        break;
                    }
                }
            }

            user->getInventory()->removerItem(item);
            *turnConsumed = true;
            return true;
        });
        return buff;
    };

    auto createMerchantFood = [](ItemID id, int healHP, int price, const std::string& desc) {
        auto food = std::make_unique<ConsumableItem>(ItemFactory::getNameDeID(id), price);
        food->addProperty(Property::HealingConsumable);
        food->setInspectionDescription(desc);
        food->setInventoryAction([healHP](Item* item, Character* user, bool* turnConsumed) {
            if (user->getHealth() >= user->getMaxHealth()) {
                showConsumableNotice("Sua health ja esta cheia!");
                return true;
            }
            int healthBefore = user->getHealth();
            user->modifyHealth(healHP);
            int healthAfter = user->getHealth();
            int actualHeal = healthAfter - healthBefore;
            showConsumableNotice(item->getItemName() + " consumido(a)! +" + std::to_string(actualHeal) + " HP. (Health atual: " + std::to_string(healthAfter) + "/" + std::to_string(user->getMaxHealth()) + ")", Color::GREEN);
            
            if (user->getQuickConsumable() == item) {
                user->unequipConsumable();
            }
            user->getInventory()->removerItem(item);
            if (turnConsumed) *turnConsumed = true;
            return true;
        });
        return food;
    };

    auto createLargeHealPotion = []() {
        auto heal = std::make_unique<ConsumableItem>(ItemFactory::getNameDeID(ItemID::LargeHealPotion), 30);
        heal->addProperty(Property::HealingConsumable);
        heal->setInspectionDescription("Restaura 50% da sua Health Maxima.");
        heal->setInventoryAction([](Item* item, Character* user, bool* turnConsumed) {
            if (user->getHealth() >= user->getMaxHealth()) {
                showConsumableNotice("Sua health ja esta cheia!");
                return true;
            }
            int healthBefore = user->getHealth();
            int estimatedHeal = static_cast<int>(user->getMaxHealth() * 0.50);
            user->modifyHealth(estimatedHeal);
            int healthAfter = user->getHealth();
            int actualHeal = healthAfter - healthBefore;
            showConsumableNotice(item->getItemName() + " usada! +" + std::to_string(actualHeal) + " HP. (Health atual: " + std::to_string(healthAfter) + "/" + std::to_string(user->getMaxHealth()) + ")", Color::GREEN);
            
            if (user->getQuickConsumable() == item) {
                user->unequipConsumable();
            }
            user->getInventory()->removerItem(item);
            if (turnConsumed) *turnConsumed = true;
            return true;
        });
        return heal;
    };

    auto createAlchemicalStrengthPotion = []() {
        auto buff = std::make_unique<ConsumableItem>(ItemFactory::getNameDeID(ItemID::AlchemicalStrengthPotion), 40);
        buff->addProperty(Property::BuffConsumable);
        buff->setInspectionDescription("Aumenta Forca em +5 e Destreza em +3 por 3 turnos em combat.");
        buff->setInventoryAction([](Item* item, Character* user, bool* turnConsumed) {
            if (!turnConsumed) { showConsumableNotice("Pocoes de buff so podem ser usadas em combat!"); return true; }
            user->addEffect(std::make_unique<WarCryEffect>(3, 5, 3));
            showConsumableNotice(item->getItemName() + " consumida! +5 Forca e +3 Destreza por 3 turnos!", Color::LIGHT_GREEN);
            
            if (user->getQuickConsumable() == item) {
                user->unequipConsumable();
            }
            user->getInventory()->removerItem(item);
            *turnConsumed = true;
            return true;
        });
        return buff;
    };

    auto createAlchemicalPoisonPotion = []() {
        auto debuff = std::make_unique<ConsumableItem>(ItemFactory::getNameDeID(ItemID::AlchemicalPoisonPotion), 35);
        debuff->addProperty(Property::WeaknessDebuffConsumable);
        debuff->setInspectionDescription("Aplica Necrose no alvo por 3 turnos (causa 12 de damage por turno).");
        debuff->setInventoryAction([](Item* item, Character* user, bool* turnConsumed) {
            if (!turnConsumed) { showConsumableNotice("Pocoes de arremesso so podem ser usadas em combat!"); return true; }
            user->setItemSelectedForUse(item);
            return true;
        });
        debuff->setUseAction([](Character* /*usuario*/, Character* target) {
            if (!Character::isValid(target) || target->getHealth() <= 0) return;
            target->addEffect(std::make_unique<NecrosisEffect>(3, 12));
            TelaCombate::adicionarMensagemFixa("\n" + TelaCombate::margemCombate() + std::string(">> Voce arremessou a pocao! ") + target->getName() + " sofreu necrose (12 damage/turno) por 3 turnos!" + std::string("\n"));
        });
        return debuff;
    };

    auto createAlchemicalSlowPotion = []() {
        auto debuff = std::make_unique<ConsumableItem>(ItemFactory::getNameDeID(ItemID::AlchemicalSlowPotion), 35);
        debuff->addProperty(Property::SlowDebuffConsumable);
        debuff->setInspectionDescription("Aplica Lentidao no alvo por 3 turnos (Reduz Destreza).");
        debuff->setInventoryAction([](Item* item, Character* user, bool* turnConsumed) {
            if (!turnConsumed) { showConsumableNotice("Pocoes de arremesso so podem ser usadas em combat!"); return true; }
            user->setItemSelectedForUse(item);
            return true;
        });
        debuff->setUseAction([](Character* /*usuario*/, Character* target) {
            if (!Character::isValid(target) || target->getHealth() <= 0) return;
            target->addEffect(std::make_unique<SlowEffect>(3));
            TelaCombate::adicionarMensagemFixa("\n" + TelaCombate::margemCombate() + std::string(">> Voce arremessou a pocao! ") + target->getName() + " esta sob efeito de Lentidao por 3 turnos!" + std::string("\n"));
        });
        return debuff;
    };

    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> constructors = {
        {ItemID::Apple, [createMerchantFood]() { return createMerchantFood(ItemID::Apple, 15, 5, "Restaura 15 HP fixo."); }},
        {ItemID::Bread, [createMerchantFood]() { return createMerchantFood(ItemID::Bread, 25, 10, "Restaura 25 HP fixo."); }},
        {ItemID::Cheese, [createMerchantFood]() { return createMerchantFood(ItemID::Cheese, 40, 18, "Restaura 40 HP fixo."); }},
        {ItemID::DriedMeat, [createMerchantFood]() { return createMerchantFood(ItemID::DriedMeat, 60, 30, "Restaura 60 HP fixo."); }},
        {ItemID::LargeHealPotion, createLargeHealPotion},
        {ItemID::AlchemicalStrengthPotion, createAlchemicalStrengthPotion},
        {ItemID::AlchemicalPoisonPotion, createAlchemicalPoisonPotion},
        {ItemID::AlchemicalSlowPotion, createAlchemicalSlowPotion},
        {ItemID::HealPotion30, createHealPotion},
        {ItemID::FuryPotion, [createAttributeBuff]() { return createAttributeBuff(ItemID::FuryPotion); }},
        {ItemID::ArcaneElixir, [createAttributeBuff]() { return createAttributeBuff(ItemID::ArcaneElixir); }},
        {ItemID::SlimeFlask, []() {
            auto debuff = std::make_unique<ConsumableItem>(ItemFactory::getNameDeID(ItemID::SlimeFlask));
            debuff->addProperty(Property::SlowDebuffConsumable);
            debuff->setInspectionDescription("Aplica Lentidao no alvo por 3 turnos (Reduz Destreza).");
            debuff->setInventoryAction([](Item* item, Character* user, bool* turnConsumed) {
                if (!turnConsumed) { showConsumableNotice("Frascos de debuff so podem ser usados em combat!"); return true; }
                user->setItemSelectedForUse(item);
                return true;
            });
            debuff->setUseAction([](Character* /*usuario*/, Character* target) {
                if (!Character::isValid(target) || target->getHealth() <= 0) return;
                target->addEffect(std::make_unique<SlowEffect>(3));
            TelaCombate::adicionarMensagemFixa("\n" + TelaCombate::margemCombate() + std::string(">> Voce jogou o frasco! ") + target->getName() + " esta com lentidao por 3 turnos!" + std::string("\n"));
            });
            return debuff;
        }},
        {ItemID::WeaknessFlask, []() {
            auto debuff = std::make_unique<ConsumableItem>(ItemFactory::getNameDeID(ItemID::WeaknessFlask));
            debuff->addProperty(Property::WeaknessDebuffConsumable);
            debuff->setInspectionDescription("Aplica Fraqueza no alvo por 3 turnos (-25% Forca).");
            debuff->setInventoryAction([](Item* item, Character* user, bool* turnConsumed) {
                if (!turnConsumed) { showConsumableNotice("Frascos de debuff so podem ser usados em combat!"); return true; }
                user->setItemSelectedForUse(item);
                return true;
            });
            debuff->setUseAction([](Character* /*usuario*/, Character* target) {
                if (!Character::isValid(target) || target->getHealth() <= 0) return;
                target->addEffect(std::make_unique<WeaknessEffect>(3));
            TelaCombate::adicionarMensagemFixa("\n" + TelaCombate::margemCombate() + std::string(">> Voce jogou o frasco! ") + target->getName() + " teve sua strength reduzida em 25% por 3 turnos!" + std::string("\n"));
            });
            return debuff;
        }},
        {ItemID::RegeneratorOrgan, []() { 
            auto buff = std::make_unique<ConsumableItem>(ItemFactory::getNameDeID(ItemID::RegeneratorOrgan), 500); 
            buff->addProperty(Property::TrollPowerConsumable); 
            buff->setInspectionDescription("Concede a regeneracao do Troll permanentemente (cura 100% HP apos batalhas).");
            buff->setInventoryAction([](Item* item, Character* user, bool* turnConsumed) {
                if (user->hasTrollRegeneration()) {
                    showConsumableNotice("O poder regenerador do Troll ja corre em suas veias!");
                } else {
                    user->unlockTrollRegeneration();
                    user->modifyHealth(user->getMaxHealth());
                    showConsumableNotice(item->getItemName() + " consumido! Voce agora heala 100% do seu HP apos cada combat!", Color::GREEN);
                    
                    if (user->getQuickConsumable() == item) user->unequipConsumable();
                    
                    user->getInventory()->removerItem(item);
                }
                if (turnConsumed) *turnConsumed = true;
                return true;
            });
            return buff; 
        }},
        {ItemID::BearTalisman, [createTalisman]() { return createTalisman(ItemID::BearTalisman, Property::StrengthTalisman, AttributeType::Strength, AttributeType::Intelligence, "Concede +5 Forca e -5 Inteligencia permanentemente."); }},
        {ItemID::RavenTalisman, [createTalisman]() { return createTalisman(ItemID::RavenTalisman, Property::IntelligenceTalisman, AttributeType::Intelligence, AttributeType::Strength, "Concede +5 Inteligencia e -5 Forca permanentemente."); }},
        {ItemID::LeopardTalisman, [createTalisman]() { return createTalisman(ItemID::LeopardTalisman, Property::DexterityTalisman, AttributeType::Dexterity, AttributeType::Wisdom, "Concede +5 Destreza e -5 Sabedoria permanentemente."); }},
        {ItemID::OwlTalisman, [createTalisman]() { return createTalisman(ItemID::OwlTalisman, Property::WisdomTalisman, AttributeType::Wisdom, AttributeType::Dexterity, "Concede +5 Sabedoria e -5 Destreza permanentemente."); }}
    };
    auto it = constructors.find(id);
    if (it != constructors.end()) return it->second();
    return nullptr;
}
