#include "InventoryController.h"
#include "Item.h"
#include "./equipment/WeaponEquipment.h"
#include "./equipment/ShieldEquipment.h"
#include "./equipment/ArmorEquipment.h"
#include "../../entities/character/Character.h"
#include <string>

ItemUsageInfo InventoryController::useOrEquip(Character* player, Item* item, bool turnAlreadyConsumed) {
    if (turnAlreadyConsumed) {
        return {ItemResult::Error_TurnAlreadyUsed, "", "", false};
    }

    if (item->isEquippable()) {
        if (item->getType() == EquipmentType::Shield && item->getShieldCurrentDurability() <= 0) {
            return {ItemResult::Error_BrokenShield, item->getItemName(), "", false};
        }

        bool unequipped = false;
        if (item == player->getWeapon()) {
            player->unequipWeapon();
            unequipped = true;
        } else if (item == player->getShield()) {
            player->unequipShield();
            unequipped = true;
        } else if (item == player->getArmor()) {
            player->unequipArmor();
            unequipped = true;
        }

        if (unequipped) {
            return {ItemResult::Unequipped, item->getItemName(), "", true};
        }

        if (!item->canBeEquippedBy(player)) {
            return {ItemResult::Error_Requirements, item->getItemName(), item->getRequirementMessage(), false};
        }

        player->equipItem(item);
        return {ItemResult::Equipped, item->getItemName(), "", true};
    }

    bool consumed = false;
    if (item->useFromInventory(player, &consumed)) {
        return {ItemResult::Used_Turn, item->getItemName(), "", consumed};
    }

    return {ItemResult::Error_CannotUse, item->getItemName(), "", false};
}

std::string InventoryController::getErrorMessage(Item* item, bool inCombat) {
    switch (item->getType()) {
        case EquipmentType::Material:
            return "Materiais sao utilizados para NPCs especializados.";
        case EquipmentType::Quest:
            return "Itens de missao sao ativados automaticamente.";
        case EquipmentType::Consumable:
            return "Este consumivel nao pode ser usado " + std::string(inCombat ? "no combat!" : "fora de combat!");
        default:
            return "Este item nao possui uso direto no inventory.";
    }
}
