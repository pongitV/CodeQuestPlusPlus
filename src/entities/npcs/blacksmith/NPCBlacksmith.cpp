#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <algorithm>
#include <memory>

#include "NPCBlacksmith.h"
#include "../../../systems/inventory/Item.h"
#include "../../../systems/inventory/ItemFactory.h"
#include "../../../systems/inventory/equipment/ArmorEquipment.h"
#include "../../../systems/inventory/equipment/ShieldEquipment.h"
#include "../../../ui/screens/inventory/ScreenInventory.h"
#include "../../../core/utils/InputControl.h"
#include "../../../core/state/Store.h"
#include "../../../core/utils/DialogFunctions.h"
#include "NPCBlacksmithLayout.h"
#include "../../../core/utils/Color.h"

namespace {
    // --- DADOS DO ESTOQUE ---
    std::map<int, StoreProduct> weaponStock = {
        {1, {ItemID::EspadaFerro, 40, -1}},
        {2, {ItemID::ArcoMadeira, 40, -1}},
        {3, {ItemID::CajadoCristal, 40, -1}},
        {4, {ItemID::ViolaoEncantado, 40, -1}}
    };
    
    std::map<int, StoreProduct> armorStock = {
        {1, {ItemID::ArmaduraMalha, 40, -1}},
        {2, {ItemID::ArmaduraCouro, 40, -1}},
        {3, {ItemID::Tunica, 40, -1}},
        {4, {ItemID::TrajeNobre, 40, -1}}
    };
    
    void processEquipmentPurchase(Character* currentPlayer, bool buyingWeapons);
    void processAnvilUpgrade(Character* currentPlayer);
    void processMaterialUpgrade(Character* currentPlayer);
    void processShieldRepair(Character* currentPlayer);

    // --- APARENCIA E DIALOGOS ---
    void bjornSingleDialogue(const std::string& msg) {
        InputControl::lerSelecaoMenuEmPopup("Bjorn", {msg}, {"OK"}, Color::CYAN, NPCBlacksmithLayouts::blacksmithArt);
    }
}

// --- INFORMACOES DO LUGAR ---
std::string NPCBlacksmith::getPlaceName() const {
    return "FORJA DO BJORN";
}

Color NPCBlacksmith::getHeaderColor() const {
    return Color::CYAN;
}

Color NPCBlacksmith::getArtColor() const {
    return Color::CYAN;
}

const std::vector<std::string>& NPCBlacksmith::getASCIIArt() const {
    return NPCBlacksmithLayouts::blacksmithArt;
}

// --- INTERACAO E MENU ---
void NPCBlacksmith::interact(Character* player) {
    InputControl::executarLoopMenuPopup(
        [this, player]() { return this->getDialogue(player); },
        [this, player]() { return this->getMenuOptions(player, 120); },
        [this, player](const std::string& op) { this->processOption(player, op, 120); return true; },
        getPlaceName(), getHeaderColor(), getASCIIArt()
    );
}

std::vector<std::string> NPCBlacksmith::getDialogue(Character* /*player*/) {
    return std::vector<std::string>{
        "Bem-vindo a minha forja, salvador!",
        "O que vai ser hoje?"
    };
}

std::vector<std::string> NPCBlacksmith::getMenuOptions(Character* /*player*/, int /*terminalWidth*/) {
    return {
        "COMPRAR Armas das Classes",
        "COMPRAR Armaduras das Classes",
        "MELHORAR POR FUSAO",
        "MELHORAR POR MATERIAL",
        "Missoes de Bjorn",
        "VOLTAR"
    };
}

void NPCBlacksmith::processOption(Character* player, const std::string& option, int /*terminalWidth*/) {
    if (option == "COMPRAR Armas das Classes" || option == "COMPRAR Armaduras das Classes") {
        processEquipmentPurchase(player, option == "COMPRAR Armas das Classes");
    } else if (option == "MELHORAR POR FUSAO") {
        processAnvilUpgrade(player);
    } else if (option == "MELHORAR POR MATERIAL") {
        processMaterialUpgrade(player);
    } else if (option == "CONSERTAR Shield") {
        processShieldRepair(player);
    } else if (option == "Missoes de Bjorn") {
        NPCInteraction::processEmptyQuestsMenu(player, "MISSOES DE BJORN", Color::CYAN, "Bjorn", "Nao tenho nenhum trabalho especial para voce no momento.");
    }
}

namespace {
    // --- PROCESSAMENTO DE OPCOES ---
    void processEquipmentPurchase(Character* currentPlayer, bool buyingWeapons) {
        auto& currentStock = buyingWeapons ? weaponStock : armorStock;
        std::string storeTitle = buyingWeapons ? "FORJA - ARMAS" : "FORJA - ARMADURAS";

        Store::processPurchase(currentPlayer, storeTitle, Color::CYAN, currentStock, 
            [](const std::string& msg) { bjornSingleDialogue(msg); }, NPCInteraction::getItemStatusFormatter, NPCBlacksmithLayouts::blacksmithArt);
    }

    void processAnvilUpgrade(Character* currentPlayer) {
        do {
            std::vector<Item*> validItems;
            std::vector<std::string> itemOptions;
            for (auto* item : currentPlayer->getInventory()->getAllItems()) {
                EquipmentType type = item->getType();
                if ((type == EquipmentType::Weapon || type == EquipmentType::Shield || type == EquipmentType::Armor) && !item->hasProperty(Property::Upgraded)) {
                    validItems.push_back(item);
                    itemOptions.push_back(item->getItemName());
                }
            }
            if (itemOptions.empty()) { bjornSingleDialogue("Voce nao tem nenhum equipamento que eu possa melhorar!"); break; }
            itemOptions.push_back("VOLTAR");
            
            int choice = InputControl::lerSelecaoMenuEmPopup("FUSAO DE EQUIPAMENTO", {"Qual item deseja fundir? (Requer copia no inventory)"}, itemOptions, Color::CYAN, NPCBlacksmithLayouts::anvilArt);
            if (choice == -1 || choice == static_cast<int>(itemOptions.size()) - 1) break;
            
            Item* baseItem = validItems[choice];
            if (!NPCInteraction::verifyItemNotEquipped(currentPlayer, baseItem, "Bjorn", Color::CYAN, "Voce precisa DESEQUIPAR o item antes de usa-lo na bigorna!")) continue;

            if (!NPCInteraction::verifyMaterialInInventory(currentPlayer, baseItem->getItemName(), 2, "Bjorn", Color::CYAN)) continue;

            if ((currentPlayer->getWeapon() && currentPlayer->getWeapon()->getItemName() == baseItem->getItemName()) ||
                (currentPlayer->getShield() && currentPlayer->getShield()->getItemName() == baseItem->getItemName()) ||
                (currentPlayer->getArmor() && currentPlayer->getArmor()->getItemName() == baseItem->getItemName())) {
                bjornSingleDialogue("Voce possui uma copia deste item equipada! DESEQUIPE antes de fundir."); continue;
            }

            std::unique_ptr<Item> newItem = baseItem->generateUpgradedCopy();

             if (newItem) {
                std::string oldName = baseItem->getItemName();
                std::string newName = newItem->getItemName();
                currentPlayer->getInventory()->removeItem(baseItem);
                currentPlayer->getInventory()->removeItem(oldName);
                currentPlayer->getInventory()->addItem(std::move(newItem));

                std::string equation = "[" + oldName + "] + [" + oldName + "] = [" + newName + "]";
            }
        } while (true);
    }

    void processMaterialUpgrade(Character* currentPlayer) {
        std::string upgradeStoneName = ItemFactory::getNameFromID(ItemID::PedraUpgrade);
        do {
            if (!NPCInteraction::verifyMaterialInInventory(currentPlayer, upgradeStoneName, 1, "Bjorn", Color::CYAN)) {
                return;
            }
            
            std::vector<Item*> validItems;
            std::vector<std::string> itemOptions;
            for (auto* item : currentPlayer->getInventory()->getAllItems()) {
                if (item->getType() == EquipmentType::Armor && !item->hasProperty(Property::UpgradedMaterial)) {
                    validItems.push_back(item);
                    itemOptions.push_back(item->getItemName());
                }
            }
            if (itemOptions.empty()) { bjornSingleDialogue("Voce nao tem armaduras validas para imbuir!"); break; }
            itemOptions.push_back("VOLTAR");
            
            int choice = InputControl::lerSelecaoMenuEmPopup("IMBUIR ARMADURA", {"Qual armadura imbuir com a Pedra? (+3 Defesa)"}, itemOptions, Color::CYAN, NPCBlacksmithLayouts::anvilArt);
            if (choice == -1 || choice == static_cast<int>(itemOptions.size()) - 1) break;

            Item* itemToUpgrade = validItems[choice];
            if (!NPCInteraction::verifyItemNotEquipped(currentPlayer, itemToUpgrade, "Bjorn", Color::CYAN, "Voce precisa DESEQUIPAR o item antes de usa-lo na bigorna!")) continue;

            ArmorEquipment* armor = dynamic_cast<ArmorEquipment*>(itemToUpgrade);
            if (!armor) continue;

            if (armor->hasProperty(Property::UpgradedMaterial)) {
                bjornSingleDialogue("Esta armadura ja foi imbuida com a pedra magica!");
                continue;
            }

            std::string oldName = armor->getItemName();
            std::string newName = oldName + " (Imbuida)";

            auto newArmor = std::make_unique<ArmorEquipment>(
                newName, 
                armor->getFlatReduction() + 3, 
                armor->getReqResistance(), 
                armor->getReqConstitution(), 
                armor->getSellPrice() + 200
            );

            for (Property prop : armor->getProperties()) newArmor->addProperty(prop);
            newArmor->addProperty(Property::UpgradedMaterial);

            currentPlayer->getInventory()->removeItem(upgradeStoneName);
            currentPlayer->getInventory()->removeItem(armor);
            currentPlayer->getInventory()->addItem(std::move(newArmor));

            std::string equation = "[" + oldName + "] + [Pedra magica] = [" + newName + "]";
        } while (true);
    }

    void processShieldRepair(Character* currentPlayer) {
        do {
            std::vector<ShieldEquipment*> damagedShields;
            std::vector<std::string> shieldOptions;

            for (auto* item : currentPlayer->getInventory()->getAllItems()) {
                ShieldEquipment* shield = dynamic_cast<ShieldEquipment*>(item);
                if (shield && shield->getCurrentShieldDurability() < shield->getMaxDurability()) {
                    damagedShields.push_back(shield);
                    int cost = (shield->getMaxDurability() - shield->getCurrentShieldDurability()) * 5;
                    shieldOptions.push_back(shield->getItemName() + " (" + std::to_string(shield->getCurrentShieldDurability()) + "/" + std::to_string(shield->getMaxDurability()) + ") - " + std::to_string(cost) + "g");
                }
            }

            if (damagedShields.empty()) {
                bjornSingleDialogue("Voce nao tem nenhum escudo danificado que eu possa consertar!");
                break;
            }
            
            shieldOptions.push_back("VOLTAR");
            
            int choice = InputControl::lerSelecaoMenuEmPopup("CONSERTAR ESCUDO", {"Qual escudo deseja consertar? (5g por ponto perdido)"}, shieldOptions, Color::CYAN, NPCBlacksmithLayouts::anvilArt);
            if (choice == -1 || choice == static_cast<int>(shieldOptions.size()) - 1) break;

            ShieldEquipment* shieldToRepair = damagedShields[choice];
            if (!NPCInteraction::verifyItemNotEquipped(currentPlayer, shieldToRepair, "Bjorn", Color::CYAN, "Voce precisa DESEQUIPAR o escudo antes de conserta-lo!")) continue;

            int lostDurability = shieldToRepair->getMaxDurability() - shieldToRepair->getCurrentShieldDurability();
            int repairCost = lostDurability * 5; // Exemplo: 5 de ouro por ponto de durabilidade perdida

            if (currentPlayer->getInventory()->getGold() >= repairCost) {
                currentPlayer->getInventory()->addGold(-repairCost);
                shieldToRepair->setDurability(shieldToRepair->getMaxDurability());
                bjornSingleDialogue("Hmph! Seu escudo esta como novo! (-" + std::to_string(repairCost) + "g)");
            } else {
                bjornSingleDialogue("Voce nao tem ouro suficiente para consertar este escudo. Eu preciso de " + std::to_string(repairCost) + "g.");
            }
        } while (true);
    }
}
