# Relatorio de Traducao: src/systems/progress

## 1. Arquivos Processados
- `ProgressionFlags.h`
- `Progression.h` & `Progression.cpp`
- `Diary.h` & `Diary.cpp`
- `Bestiary.h` & `Bestiary.cpp`

## 2. Mapeamento de Identificadores

### Classes, Estruturas e Flags:
- `Flags`: `Village_NPCs`, `Village_Enemies`, `Village_RoyalInvitation`, `Village_BjornRescued`, `Forest_NPCs`, `Forest_MorganaQuest`, `Forest_MahoragaDefeated`, `KingdomBridge_TrollDefeated`, `KingdomBridge_NPCs`, `KingdomBridge_Enemies`, `Visited_Forest`, `Visited_KingdomBridge`, `Visited_Kingdom`, `Discovered_Maps` com aliases legados.
- `Progression`: `instance`, `getInstance`, `setFlag`, `getFlag`, `getVillageProgress`, `getForestProgress`, `getKingdomBridgeProgress`, `getKingdomProgress`, `save`, `load`.
- `Diary`: `instance`, `registerItem`, `registerNPC`, `registerRace`, `registerClass`, `registerAcceptedQuest`, `registerCompletedQuest`, `isItemDiscovered`, `isNPCDiscovered`, `isRaceDiscovered`, `isClassDiscovered`, `isQuestAccepted`, `isQuestCompleted`, `getDiscoveredItems`, `getDiscoveredNPCs`, `getDiscoveredRaces`, `getDiscoveredClasses`, `getAcceptedQuests`, `getCompletedQuests`, `save`, `load`.
- `BestiaryEnemyInfo` (alias `SistemaBestiarioEnemyInfo`): `name`, `map`, `habitat`, `appearance`, `lore`, `funFact`, `attributesText`, `activeAbilities`, `passiveAbility`, `drops`, `difficulty`.
- `Bestiary`: `instance`, `initializeEnemies`, `registerFirstSight`, `registerDefeat`, `registerAbilitySeen`, `registerDrop`, `isDiscovered`, `isDefeated`, `getDefeatCount`, `hasSeenAbility`, `hasCollectedDrop`, `getInfo`, `getEnemiesOrderedByDifficulty`, `save`, `load`.

## 3. Observacoes e Integridade
- Strings literais e logs mantidos rigorosamente intactos.
- Comentarios explicativos em portugues preservados.
- Retrocompatibilidade assegurada via metodos e aliases inline.
