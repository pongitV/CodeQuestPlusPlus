#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <map>

#include "NPCMageNPC.h"
#include "../../../ui/screens/menu/ScreenMenu.h"
#include "../../../systems/inventory/Item.h"
#include "../../../systems/inventory/ItemFactory.h"
#include "../../../ui/screens/inventory/ScreenInventory.h"
#include "../../../core/utils/InputControl.h"
#include "../../../systems/inventory/equipment/WeaponEquipment.h"
#include "../../../core/state/Store.h"
#include "../../../core/utils/DialogFunctions.h"
#include "../../../systems/progress/Diary.h"
#include "../../../systems/progress/Progression.h"
#include "../../../systems/progress/ProgressionFlags.h"
#include "../../../ui/screens/ScreenBase.h"
#include "NPCMageNPCLayout.h"
#include "../../../core/utils/Color.h"

namespace {
    std::map<int, StoreProduct> buffPotionsStock = {
        {1, {ItemID::PocaoFuria, 25, -1}},
        {2, {ItemID::ElixirArcano, 25, -1}}
    };

    std::map<int, StoreProduct> debuffPotionsStock = {
        {1, {ItemID::FrascoGosma, 30, -1}},
        {2, {ItemID::FrascoFraqueza, 30, -1}}
    };

    struct EnchantOperation {
        std::string menuName;
        ItemID materialId;
        int amount;
        ItemID restrictedWeaponId; 
        std::function<bool(WeaponEquipment*)> checkConflict;
        std::string conflictMessage;
        std::function<std::string(Character*, WeaponEquipment*)> apply;
    };

    const std::vector<EnchantOperation> enchantmentOperations = {
        { "Sangramento (40x Dente de Goblin)", ItemID::DenteGoblin, 40, ItemID::None, 
          [](WeaponEquipment* a){ return a->hasBleedEffect(); }, "Esta arma ja esta encantada com Sangramento!",
          [](Character*, WeaponEquipment* a){ a->applyBleedEffect(); a->changeName(a->getItemName() + " (Sangrenta)"); return a->getItemName(); } },
          
        { "Lentidao (5x Nucleo pegajoso)", ItemID::NucleoPegajoso, 5, ItemID::None,
          [](WeaponEquipment* a){ return a->hasSlowEffect(); }, "Esta arma ja esta encantada com Lentidao!",
          [](Character*, WeaponEquipment* a){ a->applySlowEffect(); a->changeName(a->getItemName() + " (Viscosa)"); return a->getItemName(); } },
          
        { "Quebra de Resistencia (25x Po magico)", ItemID::PoMagico, 25, ItemID::None,
          [](WeaponEquipment* a){ return a->hasProperty(Property::Piercing); }, "Esta arma ja esta encantada com Reducao de Resistencia!",
          [](Character*, WeaponEquipment* a){ a->changeName(a->getItemName() + " (Quebra-Defesas)"); a->addProperty(Property::Piercing); return a->getItemName(); } },
          
        { "Arco recurvo de madeira: Magia (1x Madeira enfeiticada)", ItemID::MadeiraEnfeiticada, 1, ItemID::ArcoMadeira,
          [](WeaponEquipment* a){ return a->hasProperty(Property::Magic); }, "Esta arma ja esta encantada com Magia!",
          [](Character* currentPlayer, WeaponEquipment* chosenWeapon) {
              std::string bowName = ItemFactory::getNameFromID(ItemID::ArcoMadeira);
              std::string name = chosenWeapon->getItemName();
              size_t pos = name.find(bowName);
              if (pos != std::string::npos) name.replace(pos, 23, "Arco recurvo de madeira enfeiticada");
              int newMagicDamage = chosenWeapon->getMagicalDamage() + (chosenWeapon->getPhysicalDamage() / 2);
              auto newBowObj = std::make_unique<WeaponEquipment>(name, chosenWeapon->getPhysicalDamage(), newMagicDamage, chosenWeapon->getReqStrength(), chosenWeapon->getReqDexterity(), chosenWeapon->getReqIntelligence(), chosenWeapon->getReqWisdom(), 0);
              WeaponEquipment* newBow = newBowObj.get();
              if (chosenWeapon->hasBleedEffect()) newBow->applyBleedEffect();
              if (chosenWeapon->hasSlowEffect()) newBow->applySlowEffect();
              if (chosenWeapon->hasProperty(Property::Piercing)) newBow->addProperty(Property::Piercing);
              newBow->addProperty(Property::Magic);

              bool wasEquipped = (currentPlayer->getWeapon() == chosenWeapon);
              if (wasEquipped) currentPlayer->unequipWeapon();
              currentPlayer->getInventory()->removeItem(chosenWeapon);
              currentPlayer->getInventory()->addItem(std::move(newBowObj));
              if (wasEquipped) currentPlayer->equipItem(newBow);
              return newBow->getItemName();
          } },
          
        { "Cajado de cristal magico: Cipos (1x Coracao da floresta)", ItemID::CoracaoFloresta, 1, ItemID::CajadoCristal,
          [](WeaponEquipment* a){ return a->hasProperty(Property::VineTrap); }, "Esta arma ja esta encantada com Cipos!",
          [](Character*, WeaponEquipment* a){
              std::string staffName = ItemFactory::getNameFromID(ItemID::CajadoCristal);
              std::string name = a->getItemName();
              size_t pos = name.find(staffName);
              if (pos != std::string::npos) name.replace(pos, 24, "Cajado de cipos");
              a->changeName(name);
              a->addProperty(Property::VineTrap);
              return a->getItemName();
          } },
          
        { "Violao encantado: Raizes (1x Madeira enfeiticada)", ItemID::MadeiraEnfeiticada, 1, ItemID::ViolaoEncantado,
          [](WeaponEquipment* a){ return a->hasProperty(Property::MagicGuitar); }, "Esta arma ja esta encantada com Raizes!",
          [](Character*, WeaponEquipment* a){
              std::string guitarName = ItemFactory::getNameFromID(ItemID::ViolaoEncantado);
              std::string name = a->getItemName();
              size_t pos = name.find(guitarName);
              if (pos != std::string::npos) name.replace(pos, 16, "Violao enfeiticado");
              else name += " enfeiticado";
              a->changeName(name);
              a->addProperty(Property::MagicGuitar);
              return a->getItemName();
          } }
    };

    // Aparencia e dialogos
    void processEnchantments(Character* currentPlayer, bool isUniversal);
    void processPotions(Character* currentPlayer, bool isBuff);
    void processLabyrinthQuest(Character* currentPlayer);
    void processQuestsMenu(Character* currentPlayer);

    void morganaSingleDialogue(const std::string& msg) {
        InputControl::lerSelecaoMenuEmPopup("Morgana", {msg}, {"OK"}, Color::CYAN, NPCMageNPCLayouts::mageArt);
    }
}

// Informacoes do lugar
std::string NPCMageNPC::getPlaceName() const {
    return "CABANA DA BRUXA";
}

Color NPCMageNPC::getHeaderColor() const {
    return Color::MAGENTA;
}

Color NPCMageNPC::getArtColor() const {
    return Color::MAGENTA;
}

const std::vector<std::string>& NPCMageNPC::getASCIIArt() const {
    return NPCMageNPCLayouts::mageArt;
}

// Interacao e menu
void NPCMageNPC::interact(Character* player) {
    InputControl::executarLoopMenuPopup(
        [this, player]() { return this->getDialogue(player); },
        [this, player]() { return this->getMenuOptions(player, 120); },
        [this, player](const std::string& op) { this->processOption(player, op, 120); return true; },
        getPlaceName(), getHeaderColor(), getASCIIArt()
    );
}

std::vector<std::string> NPCMageNPC::getDialogue(Character* /*jogador*/) {
    if (Progression::instance().getFlag(Flags::Floresta_MissaoMorgana)) {
        return std::vector<std::string>{
            "O Labirinto o aguarda..."
        };
    } else {
        return std::vector<std::string>{
            "Hmmm... sinto cheiro de poder no ar.",
            "O que voce busca, viajante?"
        };
    }
}

std::vector<std::string> NPCMageNPC::getMenuOptions(Character* /*jogador*/, int /*larguraTerminal*/) {
    return {
        "ENCANTAR Armas (Universais)",
        "ENCANTAR Armas (Especificas)",
        "COMPRAR Pocoes de Buff",
        "COMPRAR Frascos de Debuff",
        "Missoes de Morgana",
        "VOLTAR"
    };
}

void NPCMageNPC::processOption(Character* player, const std::string& option, int /*larguraTerminal*/) {
    if (option == "ENCANTAR Armas (Universais)") {
        processEnchantments(player, true);
    }
    else if (option == "ENCANTAR Armas (Especificas)") {
        processEnchantments(player, false);
    }
    else if (option == "COMPRAR Pocoes de Buff" || option == "COMPRAR Frascos de Debuff") {
        processPotions(player, option == "COMPRAR Pocoes de Buff");
    }
    else if (option == "Missoes de Morgana") {
        processQuestsMenu(player);
    }
}

namespace {
    // Processamento de opcoes
    void processEnchantments(Character* currentPlayer, bool isUniversal) {
        std::vector<const EnchantOperation*> currentOps;
        int start = isUniversal ? 0 : 3;
        int end = isUniversal ? 3 : 6;
        for (int i = start; i < end; ++i) {
            currentOps.push_back(&enchantmentOperations[i]);
        }

        while (true) {
            std::vector<std::string> lines;
            for (auto* op : currentOps) lines.push_back(op->menuName);
            lines.push_back("VOLTAR");

            int id = InputControl::lerSelecaoMenuEmPopup(
                isUniversal ? "CABANA - ENCANTOS UNIVERSAIS" : "CABANA - ENCANTOS ESPECIFICOS",
                {"Escolha um encantamento:"},
                lines,
                Color::MAGENTA,
                NPCMageNPCLayouts::mageArt
            );

            if (id == static_cast<int>(currentOps.size()) || id == -1) {
                break;
            }

            const auto& op = *currentOps[id];
            
            std::string requiredItem = ItemFactory::getNameFromID(op.materialId);
            int currentAmount = currentPlayer->getInventory()->getItemCount(requiredItem);
            if (currentAmount < op.amount) {
                morganaSingleDialogue("Voce nao tem " + requiredItem + " suficiente! (Possui: " + std::to_string(currentAmount) + "/" + std::to_string(op.amount) + ")");
                continue;
            }
            
            std::vector<Item*> validItems;
            std::vector<std::string> itemOptions;
            for (auto* item : currentPlayer->getInventory()->getAllItems()) {
                if (item->getType() == EquipmentType::Weapon) {
                    validItems.push_back(item);
                    itemOptions.push_back(item->getItemName());
                }
            }
            if (itemOptions.empty()) { morganaSingleDialogue("Voce nao tem armas para encantar!"); continue; }
            itemOptions.push_back("VOLTAR");
            
            int weaponChoice = InputControl::lerSelecaoMenuEmPopup("ESCOLHA UMA ARMA", {"Qual arma deseja encantar?"}, itemOptions, Color::MAGENTA, NPCMageNPCLayouts::cauldronArt);
            if (weaponChoice == -1 || weaponChoice == static_cast<int>(itemOptions.size()) - 1) continue;
            
            WeaponEquipment* chosenWeapon = dynamic_cast<WeaponEquipment*>(validItems[weaponChoice]);
            
            if (op.restrictedWeaponId != ItemID::None) {
                std::string restrictedName = ItemFactory::getNameFromID(op.restrictedWeaponId);
                if (chosenWeapon->getItemName().find(restrictedName) == std::string::npos) {
                    morganaSingleDialogue("Este encantamento so funciona no " + restrictedName + "!");
                    continue;
                }
            }
            
            if (op.checkConflict(chosenWeapon)) {
                morganaSingleDialogue(op.conflictMessage);
                continue;
            }
            
            std::string oldWeaponName = chosenWeapon->getItemName();
            for (int i = 0; i < op.amount; ++i) currentPlayer->getInventory()->removeItem(requiredItem);
            
            std::string newName = op.apply(currentPlayer, chosenWeapon);
            
            std::string equation = "[" + oldWeaponName + "] + " + std::to_string(op.amount) + "x [" + requiredItem + "] = [" + newName + "]";
        }
    }

    void processPotions(Character* currentPlayer, bool isBuff) {
        std::string title = isBuff ? "CABANA - POCOES DE BUFF" : "CABANA - FRASCOS DE DEBUFF";
        auto& currentStock = isBuff ? buffPotionsStock : debuffPotionsStock;
        
        Store::processPurchase(currentPlayer, title, Color::MAGENTA, currentStock, 
            [](const std::string& msg) { morganaSingleDialogue(msg); }, NPCInteraction::getItemStatusFormatter, NPCMageNPCLayouts::mageArt);
    }

    void processLabyrinthQuest(Character* currentPlayer) {
        std::string heartName = ItemFactory::getNameFromID(ItemID::CoracaoFloresta);
        int heartCount = currentPlayer->getInventory()->getItemCount(heartName);

        if (heartCount < 3) {
            morganaSingleDialogue("Voce ainda nao possui os 3 Coracoes da floresta que eu pedi. (Possui: " + std::to_string(heartCount) + "/3)\nEles sao dropados por Abominacoes no Coracao da Arvore.");
            return;
        }

        for (int i = 0; i < 3; ++i) currentPlayer->getInventory()->removeItem(heartName);
        currentPlayer->unlockLabyrinth();
        Diary::instance().registerQuestCompleted("morgana_coracoes");
        Progression::instance().setFlag(Flags::Floresta_MissaoMorgana, true);
        
        std::vector<std::string> dialogue = {
            "Ah, perfeitos! Estes coracoes pulsam com uma magia ancestral.",
            "Como recompensa, revelarei um segredo... Atras de mim, ha uma passagem secreta.",
            "Use a entrada [^L] para explorar o meu Labirinto Subterraneo.",
            "E um lugar perigoso, mergulhado em uma nevoa de cor roxa, mas guarda grandes tesouros."
        };
    }

    void processQuestsMenu(Character* currentPlayer) {
        Diary::instance().registerQuestAccepted("morgana_coracoes");
        while (true) {
            std::vector<std::string> quests;
            if (!currentPlayer->isLabyrinthUnlocked()) {
                std::string heartName = ItemFactory::getNameFromID(ItemID::CoracaoFloresta);
                int heartCount = currentPlayer->getInventory()->getItemCount(heartName);
                if (heartCount >= 3) {
                    quests.push_back("[M] Entregar 3x Coracoes da floresta (Pronta)");
                } else {
                    quests.push_back("[M] Consiga 3x Coracoes da floresta");
                }
            } else {
                quests.push_back("(Nenhuma missao disponivel)");
            }
            quests.push_back("VOLTAR");

            int id = InputControl::lerSelecaoMenuEmPopup(
                "MISSOES DE MORGANA",
                {"Escolha uma missao:"},
                quests,
                Color::MAGENTA,
                NPCMageNPCLayouts::mageArt
            );

            if (!currentPlayer->isLabyrinthUnlocked() && id == 0) {
                processLabyrinthQuest(currentPlayer);
            } else if (currentPlayer->isLabyrinthUnlocked() && id == 0) {
                morganaSingleDialogue("Nao busco mais nada de voce no momento...");
            } else if (id == 1 || id == -1) {
                break;
            }
        }
    }
}
