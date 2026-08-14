#include "ItemFactory.h"
#include "./equipment/WeaponEquipment.h"
#include "./equipment/ShieldEquipment.h"
#include "./equipment/ArmorEquipment.h"
#include "./items/ConsumableItem.h"
#include "./items/MaterialItem.h"
#include "./items/MissionItem.h"

#include "../../entities/character/Character.h"
#include <functional>
#include <utility>
#include <unordered_map>

namespace {
    const std::pair<const char*, ItemID> globalNamesMap[] = {
        {"Adaga artesanal de pedra", ItemID::StoneDagger},
        {"Arco recurvo de madeira", ItemID::WoodBow},
        {"Cajado de cristal magico", ItemID::CrystalStaff},
        {"Varinha corroida", ItemID::CorrodedWand},
        {"Violao encantado", ItemID::EnchantedGuitar},
        {"Cajado de osso", ItemID::BoneStaff},
        {"Espada longa de ferro", ItemID::IronSword},
        {"Machado de guerra danificado", ItemID::BattleAxe},
        {"Gosma acida corrosiva", ItemID::AcidSlimeWeapon},
        {"Tronco de arvore amarrotado", ItemID::CrumpledTrunk},
        {"Espada do Cavaleiro", ItemID::KnightSword},
        
        {"Shield medio de metal", ItemID::MetalShield},
        {"Barreira magica", ItemID::MagicBarrier},
        {"Capa magica", ItemID::MagicCape},
        {"Bracedeiras de prata", ItemID::SilverBracers},
        
        {"Armor de malha e metal", ItemID::ChainArmor},
        {"Armor leve de couro com malha", ItemID::LeatherArmor},
        {"Tunica", ItemID::Tunic},
        {"Traje de Couro e tecido nobre", ItemID::NobleOutfit},
        {"Armor de trapos e sucata", ItemID::RagsArmor},
        {"Armor de Cavaleiro", ItemID::KnightArmor},
        {"Roupas de Ritualista", ItemID::RitualistClothes},
        {"Armor de bau", ItemID::ChestArmor},
        {"Roda da Adaptacao", ItemID::AdaptationWheel},
        
        {"Pocao de Cura (30%VM)", ItemID::HealPotion30},
        {"Pocao de Furia (Buff)", ItemID::FuryPotion},
        {"Elixir Arcano (Buff)", ItemID::ArcaneElixir},
        {"Frasco de Gosma (Debuff)", ItemID::SlimeFlask},
        {"Frasco de Fraqueza (Debuff)", ItemID::WeaknessFlask},
        {"Orgao regenerador", ItemID::RegeneratorOrgan},
        {"Talisma do Urso", ItemID::BearTalisman},
        {"Talisma do Corvo", ItemID::RavenTalisman},
        {"Talisma do Leopardo", ItemID::LeopardTalisman},
        {"Talisma da Coruja", ItemID::OwlTalisman},
        {"Maca", ItemID::Apple},
        {"Pao", ItemID::Bread},
        {"Queijo", ItemID::Cheese},
        {"Carne Seca", ItemID::DriedMeat},
        {"Pocao de Cura Grande (50%VM)", ItemID::LargeHealPotion},
        {"Pocao de Forca Alquimica", ItemID::AlchemicalStrengthPotion},
        {"Pocao de Veneno Alquimica (Debuff)", ItemID::AlchemicalPoisonPotion},
        {"Pocao de Lentidao Alquimica (Debuff)", ItemID::AlchemicalSlowPotion},
        
        {"Gosma acida", ItemID::AcidSlime},
        {"Dente de goblin", ItemID::GoblinTooth},
        {"Nucleo pegajoso", ItemID::StickyCore},
        {"Po magico", ItemID::MagicDust},
        {"Madeira enfeiticada", ItemID::EnchantedWood},
        {"Coracao da floresta", ItemID::ForestHeart},
        {"Pedra magica de upgrade", ItemID::UpgradeStone},
        {"Convite Real", ItemID::RoyalInvitation},
        
        {"Dispositivo de teclas de linguagem desconhecida", ItemID::LanguageDevice}
    };

    struct ItemMaps {
        std::unordered_map<ItemID, std::string> idToName;
        std::unordered_map<std::string, ItemID> nameToId;
        ItemMaps() {
            for (const auto& pair : globalNamesMap) {
                idToName[pair.second] = pair.first;
                nameToId[pair.first] = pair.second;
            }
        }
    };

    const ItemMaps& getItemMaps() {
        static ItemMaps maps;
        return maps;
    }
}

std::string ItemFactory::getNameFromID(ItemID id) {
    const auto& map = getItemMaps().idToName;
    auto it = map.find(id);
    return it != map.end() ? it->second : "";
}

ItemID ItemFactory::getIDFromName(const std::string& name) {
    const auto& map = getItemMaps().nameToId;
    auto it = map.find(name);
    return it != map.end() ? it->second : ItemID::None;
}

std::vector<std::unique_ptr<Item>> ItemFactory::createMultipleItems(ItemID id, int amount) {
    std::vector<std::unique_ptr<Item>> items;
    items.reserve(amount);
    for (int i = 0; i < amount; ++i) {
        items.push_back(createItem(id));
    }
    return items;
}

std::vector<std::unique_ptr<Item>> ItemFactory::createPotionKit(int amount) {
    return createMultipleItems(ItemID::HealPotion30, amount);
}

std::unique_ptr<Item> ItemFactory::createItem(const std::string& name) 
{
    if (!name.empty() && name.back() == '+') {
        std::string baseName = name.substr(0, name.length() - 1);
        auto baseItem = createItem(baseName);
        if (baseItem) return baseItem->generateUpgradedCopy();
        return nullptr;
    }

    ItemID id = getIDFromName(name);
    if (id != ItemID::None) return createItem(id);

    return nullptr;
}

std::unique_ptr<Item> ItemFactory::createItem(ItemID id) {
    static const std::vector<std::function<std::unique_ptr<Item>(ItemID)>> manufacturerChain = {
        buildWeaponEquipment, buildShieldEquipment, buildArmorEquipment,
        buildConsumableItem, buildMaterialItem, buildMissionItem
    };
    for (const auto& manufacturer : manufacturerChain) {
        if (auto item = manufacturer(id)) return item;
    }
    return nullptr;
}
