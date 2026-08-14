# Relatorio de Traducao: src/systems/inventory

## 1. Arquivos Processados
- `Item.h`
- `ItemFactory.h` & `ItemFactory.cpp`
- `Inventory.h` & `Inventory.cpp`
- `InventoryController.h` & `InventoryController.cpp`
- `InventoryCombat.h` & `InventoryCombat.cpp`
- `equipment/BaseEquipment.h`
- `equipment/ArmorEquipment.h` & `equipment/ArmorEquipment.cpp`
- `equipment/ShieldEquipment.h` & `equipment/ShieldEquipment.cpp`
- `equipment/WeaponEquipment.h` & `equipment/WeaponEquipment.cpp`
- `items/ConsumableItem.h` & `items/ConsumableItem.cpp`
- `items/MaterialItem.h` & `items/MaterialItem.cpp`
- `items/MissionItem.h` & `items/MissionItem.cpp`

## 2. Mapeamento de Identificadores

### Enums e Tipos:
- `EquipmentType` (`None`, `Weapon`, `Shield`, `Armor`, `Consumable`, `Quest`, `Material`) com alias `TipoEquipamento`.
- `Property` (`None`, `Magic`, `Piercing`, `IgnoreDefense`, `BasicGuitar`, `MagicGuitar`, `VineTrap`, `Upgraded`, `UpgradedMaterial`, `HealingConsumable`, `BuffConsumable`, `SlowDebuffConsumable`, `WeaknessDebuffConsumable`, `StrengthTalisman`, `IntelligenceTalisman`, `DexterityTalisman`, `WisdomTalisman`, `TrollPowerConsumable`, `AdaptationArmor`) com alias `Propriedade`.
- `ItemID` (`None`, `StoneDagger`, `WoodBow`, `CrystalStaff`, `CorrodedWand`, `EnchantedGuitar`, `IronSword`, `BattleAxe`, `AcidSlimeWeapon`, `CrumpledTrunk`, `KnightSword`, `ExterminationSword`, `BoneStaff`, `MetalShield`, `MagicBarrier`, `MagicCape`, `SilverBracers`, `ChainArmor`, `LeatherArmor`, `Tunic`, `NobleOutfit`, `RagsArmor`, `KnightArmor`, `ChestArmor`, `AdaptationWheel`, `RitualistClothes`, `HealPotion30`, `FuryPotion`, `ArcaneElixir`, `SlimeFlask`, `WeaknessFlask`, `RegeneratorOrgan`, `BearTalisman`, `RavenTalisman`, `LeopardTalisman`, `OwlTalisman`, `Apple`, `Bread`, `Cheese`, `DriedMeat`, `LargeHealPotion`, `AlchemicalStrengthPotion`, `AlchemicalPoisonPotion`, `AlchemicalSlowPotion`, `AcidSlime`, `GoblinTooth`, `StickyCore`, `MagicDust`, `EnchantedWood`, `ForestHeart`, `UpgradeStone`, `RoyalInvitation`, `LanguageDevice`) com aliases legados.
- `ItemResult` (`Equipped`, `Unequipped`, `Used_Turn`, `Used_NoTurn`, `Error_TurnAlreadyUsed`, `Error_BrokenShield`, `Error_Requirements`, `Error_CannotUse`, `None`) com alias `ResultadoItem`.
- `ItemUsageInfo` com alias `UsoItemInfo`.
- `InventoryState` (`MAIN`, `ARSENAL`, `CONSUMABLES`, `STORAGE`, `QUEST`).

### Classes e Metodos:
- `Item`: `getItemName`, `getType`, `getPhysicalDamage`, `getMagicalDamage`, `getPercentageReduction`, `getFixedReduction`, `getShieldFixedDamageReduction`, `getShieldCurrentDurability`, `setInspectionDescription`, `checkAttributeRequirements`, `checkArmorRequirements`, `checkShieldRequirements`, `canBeEquippedBy`, `isEquippable`, `getRequirementMessage`, `getInspectionDetails`, `changeName`, `hasBleedEffect`, `hasSlowEffect`, `applyBleedEffect`, `applySlowEffect`, `reduceDurability`, `increaseDurability`, `beforeDealingDamage`, `onDealingDamage`, `ensureMinimumDamage`, `getSellPrice`, `getStatusInfo`, `use`, `setUseAction`, `setInventoryAction`, `useFromInventory`, `hasProperty`, `addProperty`, `removeProperty`, `getProperties`, `generateUpgradedCopy`.
- `BaseEquipment`: `checkSpecificRequirements`, `getBaseInspectionDetails`.
- `ArmorEquipment`: `getReqResistance`, `getReqConstitution`, `setDexterityPenalty`, `buildArmorEquipment`.
- `ShieldEquipment`: `getReqResistance`, `getReqSecondary`, `getSecondaryType`, `getMaxDurability`, `setDurability`, `buildShieldEquipment`.
- `WeaponEquipment`: `getReqStrength`, `getReqDexterity`, `getReqIntelligence`, `getReqWisdom`, `buildWeaponEquipment`.
- `ConsumableItem`, `MaterialItem`, `MissionItem`: `buildConsumableItem`, `buildMaterialItem`, `buildMissionItem`.
- `ItemFactory`: `createItem`, `createMultipleItems`, `createPotionKit`, `getNameFromID`, `getIDFromName`.
- `Inventory`: `getAllItems`, `getSortedIndices`, `isEmpty`, `getGold`, `countItem`, `addGold`, `addItem`, `removeItem`, `findItemByCode`.
- `InventoryController`: `useOrEquip`, `getErrorMessage`.
- `InventoryCombat`: `manageInventory`.

## 3. Observacoes e Integridade
- Strings literais, caminhos e dialogos foram rigorosamente preservados intactos.
- Comentarios explicativos atualizados e mantidos em portugues.
- Retrocompatibilidade assegurada com aliases e funcoes delegadas inline.
