# Core Folder Translation Documentation (Portuguese)

## Overview
This document describes the translation work performed on the `src/core` folder of the CodeQuestPlusPlus project.

## Translation Rules Applied
1. **Function Names**: Translated from Portuguese to English based on functionality
2. **Comments**: Kept in Portuguese (no translation)
3. **Strings**: Kept in Portuguese (no translation)

## Files Translated

### config/
- **Config.h**: No changes needed (already in English)
- **Config.cpp**: No changes needed (already in English)

### d2d-context/
- **D2DContext.h**: No changes needed (already in English)
- **D2DContext.cpp**: No changes needed (already in English)

### input/
- **IInputBackend.h**: No changes needed (already in English)
- **InputSystem.h**: No changes needed (already in English)
- **InputSystem.cpp**: No changes needed (already in English)
- **Win32Input.h**: Translated function names
  - `teclaPressionada` → `isKeyPressed`
  - `teclaPressionadaAgora` → `isKeyJustPressed`
  - `mouseMovendo` → `isMouseMoving`
  - `obterMouseX` → `getMouseX`
  - `obterMouseY` → `getMouseY`
  - `limpar` → `clear`
- **Win32Input.cpp**: Translated function names (same as .h file)

### logger/
- **Logger.h**: No changes needed (already in English)
- **Logger.cpp**: No changes needed (already in English)

### state/
- **Debug.h**: Translated function names
  - `isGodModeAtivo` → `isGodModeActive`
  - `isNoclipAtivo` → `isNoclipActive`
  - `isOneHitKillAtivo` → `isOneHitKillActive`
  - `isSpeedHackAtivo` → `isSpeedHackActive`
  - `displayMenuDebug` → `displayDebugMenu`
  - `isTeclaDebug` → `isDebugKey`
- **Debug.cpp**: No functional changes (kept Portuguese strings in comments/outputs)
- **Drops.h**: Translated function names
  - `relatarEProcessarXpOuro` → `reportAndProcessXPGold`
  - `relatarDropItem` → `reportItemDrop`
  - `darEProcessarItem` → `giveAndProcessItem`
- **Drops.cpp**: Translated function names (same as .h file)
- **EnemyCreator.h**: Translated function names
  - `criarInimigoGoblin` → `createGoblinEnemy`
  - `criarInimigoSlime` → `createSlimeEnemy`
  - `criarInimigoFada` → `createFairyEnemy`
  - `criarInimigoOrkExilado` → `createExiledOrcEnemy`
  - `criarInimigoAbominacaoFloresta` → `createForestAbominationEnemy`
  - `criarInimigoTroll` → `createTrollEnemy`
  - `criarInimigoMimic` → `createMimicEnemy`
  - `criarInimigoMahoraga` → `createMahoragaEnemy`
- **EnemyCreator.cpp**: Translated function names (same as .h file)
- **GameMenu.h**: Translated function names
  - `menuPrincipal` → `mainMenu`
  - `iniciarCriacaoDeSistemaPersonagem` → `startCharacterCreationSystem`
- **GameMenu.cpp**: Translated function names (same as .h file)
- **MapManager.h**: Translated function names
  - `armazenarMapa` → `cacheMap`
  - `possuiMapa` → `hasMap`
  - `obterMapa` → `getMap`
- **MapManager.cpp**: No changes needed (already in English)
- **StateManager.h**: No changes needed (already in English)
- **StateManager.cpp**: No changes needed (already in English)
- **Status.h**: No changes needed (already in English)
- **Store.h**: Translated function names
  - `processarCompra` → `processPurchase`
- **Store.cpp**: Translated function names (same as .h file)

## Notes
- All function names have been translated to English where they were in Portuguese
- Comments remain in Portuguese as per the translation rules
- Strings (user-facing text) remain in Portuguese as per the translation rules
- The translation maintains backward compatibility where enum values and other identifiers are kept

## Next Steps
The next folders to be translated are:
- entities/
- maps/
- rendering/
- systems/
- ui/
