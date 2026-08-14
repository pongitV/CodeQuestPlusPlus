# Relatório de Tradução: src/entities/npcs/

## Arquivos Processados
- `NPCInteraction.h` e `NPCInteraction.cpp`
- `appearance/NPCAppearance.h`, `appearance/NPCAppearance.cpp` e `appearance/NPCAppearanceLayout.h`
- `alchemist/NPCAlchemist.h`, `alchemist/NPCAlchemist.cpp` e `alchemist/NPCAlchemistLayout.h`
- `blacksmith/NPCBlacksmith.h`, `blacksmith/NPCBlacksmith.cpp` e `blacksmith/NPCBlacksmithLayout.h`
- `food-merchant/NPCFoodMerchant.h`, `food-merchant/NPCFoodMerchant.cpp` e `food-merchant/NPCFoodMerchantLayout.h`
- `generic-knight/NPCGenericKnight.h`, `generic-knight/NPCGenericKnight.cpp` e `generic-knight/NPCGenericKnightLayout.h`
- `mage-npc/NPCMageNPC.h`, `mage-npc/NPCMageNPC.cpp` e `mage-npc/NPCMageNPCLayout.h`
- `merchant/NPCMerchant.h`, `merchant/NPCMerchant.cpp` e `merchant/NPCMerchantLayout.h`
- `priest/NPCPriest.h`, `priest/NPCPriest.cpp` e `priest/NPCPriestLayout.h`

## Mapeamento de Identificadores (Português -> Inglês)

### Classes e Interfaces
- `NPCInteraction`: métodos virtuais e utilitários estáticos renomeados (`getPlaceName`, `getHeaderColor`, `getArtColor`, `getASCIIArt`, `getDialogue`, `getMenuOptions`, `processOption`, `interact`, `processEmptyQuestsMenu`, `verifyMaterialInInventory`, `verifyItemNotEquipped`, `readItemFromInventory`, `displaySuccessScreen`, `getItemStatusFormatter`).
- `NPCAparencia` -> `NPCAppearance` (com alias `using NPCAparencia = NPCAppearance;`)
- `NPCAlquimista` -> `NPCAlchemist` (com alias `using NPCAlquimista = NPCAlchemist;`)
- `NPCFerreiro` -> `NPCBlacksmith` (com alias `using NPCFerreiro = NPCBlacksmith;`)
- `NPCFood` -> `NPCFoodMerchant` (com alias `using NPCFood = NPCFoodMerchant;`)
- `NPCCavaleiroGenerico` -> `NPCGenericKnight` (com alias `using NPCCavaleiroGenerico = NPCGenericKnight;`)
- `NPCMago` -> `NPCMageNPC` (com alias `using NPCMago = NPCMageNPC;`)
- `NPCMercador` -> `NPCMerchant` (com alias `using NPCMercador = NPCMerchant;`)
- `NPCClerigo` -> `NPCPriest` (com alias `using NPCClerigo = NPCPriest;`)

### Layouts e Arte ASCII
- `NPCAparenciaLayouts` / `NPCAppearanceLayouts`: `arteAparencia` / `appearanceArt`
- `NPCAlquimistaLayouts` / `NPCAlchemistLayouts`: `arteAlchemist` / `alchemistArt`
- `NPCFerreiroLayouts` / `NPCBlacksmithLayouts`: `arteBlacksmith` / `blacksmithArt`, `arteBigorna` / `anvilArt`
- `NPCFoodMerchantLayouts`: `foodMerchantArt` / `arteFoodMerchant`
- `NPCCavaleiroGenericoLayouts` / `NPCGenericKnightLayouts`: `arteCavaleiro` / `knightArt`
- `NPCMagoLayouts` / `NPCMageNPCLayouts`: `arteMageNPC` / `mageArt`, `arteCaldeirao` / `cauldronArt`
- `NPCMercadorLayouts` / `NPCMerchantLayouts`: `arteMerchant` / `merchantArt`
- `NPCClerigoLayouts` / `NPCPriestLayouts`: `artePriest` / `priestArt`

### Métodos e Funções Internas
- `processPurchaseDeEquipamento` -> `processEquipmentPurchase`
- `processarMelhoriaNaBigorna` -> `processAnvilUpgrade`
- `processarUpgradePorMaterial` -> `processMaterialUpgrade`
- `processarConsertoDeEscudo` -> `processShieldRepair`
- `processarEncantamentos` -> `processEnchantments`
- `processarPocoes` -> `processPotions`
- `processarMissaoLabirinto` -> `processLabyrinthQuest`
- `processarMenuMissoes` -> `processQuestsMenu`
- `processarVendaDeItens` -> `processItemSales`

## Observações de Integridade
- Todas as strings literais, diálogos e mensagens para o usuário foram mantidas estritamente intactas.
- Comentários foram preservados em português.
- Compilação realizada com sucesso via CMake.
