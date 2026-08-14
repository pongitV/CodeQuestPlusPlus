# Relatório de Tradução: src/ui

## 1. Arquivos Processados
- `UIManager.h` / `UIManager.cpp`
- `UIRenderer.h` / `UIRenderer.cpp`
- `screens/ScreenBase.h` / `ScreenBase.cpp`
- `screens/Screen3DScene.h` / `Screen3DScene.cpp`
- `screens/attributes/ScreenAttributes.h` / `ScreenAttributes.cpp`
- `screens/attributes/ScreenAttributesLayout.h`
- `screens/bestiary/ScreenBestiary.h` / `ScreenBestiary.cpp`
- `screens/combat/CombatContext.h`
- `screens/combat/ScreenCombat.h` / `ScreenCombat.cpp`
- `screens/defeat/ScreenDefeat.h` / `ScreenDefeat.cpp`
- `screens/diary/ScreenDiary.h` / `ScreenDiary.cpp`
- `screens/diary/ScreenDiaryLogic.h` / `ScreenDiaryLogic.cpp`
- `screens/inventory/ScreenInventory.h` / `ScreenInventory.cpp`
- `screens/map-world/ScreenMapWorld.h` / `ScreenMapWorld.cpp`
- `screens/menu/ScreenMenu.h` / `ScreenMenu.cpp`
- `screens/menu/ScreenMenuBase.h` / `ScreenMenuBase.cpp`
- `screens/menu/ScreenName.h` / `ScreenName.cpp`
- `screens/menu/ScreenRace.h` / `ScreenRace.cpp`
- `screens/menu/ScreenClass.h` / `ScreenClass.cpp`
- `screens/menu/ScreenDifficulty.h` / `ScreenDifficulty.cpp`
- `screens/menu/ScreenParry.h` / `ScreenParry.cpp`
- `screens/menu/ScreenOpening.h` / `ScreenOpening.cpp`
- `screens/menu/ScreenTutorial.h` / `ScreenTutorial.cpp`
- `screens/menu/ScreenIntroduction.h` / `ScreenIntroduction.cpp`
- `screens/pause/ScreenPause.h` / `ScreenPause.cpp`
- `screens/victory/ScreenVictory.h` / `ScreenVictory.cpp`
- `screens/transition/ScreenTransition.h`
- `screens/utils/ScreenRegistry.h` / `ScreenRegistry.cpp`
- `screens/interfaces/IScreenManager.h`
- `interfaces/IAttributesUI.h`
- `interfaces/IBestiaryUI.h`
- `interfaces/IDiaryUI.h`
- `interfaces/IInventoryUI.h`
- `interfaces/IScreenCombatUI.h`
- `interfaces/IDefeatUI.h`
- `interfaces/IVictoryUI.h`
- `interfaces/IPauseUI.h`
- `interfaces/IMapWorldUI.h`

## 2. Mapeamento de Identificadores

### UIManager:
- `GerenciadorPerspectiva` -> `PerspectiveManager`:
  - `obterInstancia` -> `getInstance`
  - `alternarVisao` -> `toggleView`
  - `isVisao3DAtiva` -> `is3DViewActive`
  - `obterDiarioUI` -> `getDiaryUI`
  - `obterInventarioUI` -> `getInventoryUI`
  - `obterAtributosUI` -> `getAttributesUI`
  - `obterBestiarioUI` -> `getBestiaryUI`
  - `obterTelaCombateUI` -> `getCombatUI`
  - `obterDerrotaUI` -> `getDefeatUI`
  - `obterVitoriaUI` -> `getVictoryUI`
  - `obterPauseUI` -> `getPauseUI`
  - `obterMapaMundoUI` -> `getWorldMapUI`

### Screens:
- `TelaBase` -> `ScreenBase`
- `TelaAtributos` -> `AttributesScreen`: `show`, `managePlayerSheet`, `PoderCombate` -> `CombatPower`
- `TelaBestiario` -> `BestiaryScreen`: `showList`
- `TelaCombate` -> `CombatScreen`
- `TelaDerrota` -> `DefeatScreen`
- `TelaDiario` -> `DiaryScreen`: `show`
- `TelaDiarioLogic`: integrando com `EquipmentType::Weapon`, `Shield`, `Armor`, `Consumable`, `Material`, `Quest`
- `TelaInventario` -> `InventoryScreen`: `showEquippedBox`, `showItemInspection`, `EquipmentType`
- `TelaMapaMundo` -> `WorldMapScreen`: `show`, `LocalizacaoMapa` -> `MapLocation`
- `TelaPause` -> `PauseScreen`: `show`
- `TelaVitoria` -> `VictoryScreen`
- `IGerenciadorTelas` -> `IScreenManager`

## 3. Observações e Ajustes
- Strings e textos de menus e diálogos mantidos 100% inalterados.
- Comentários e documentações mantidos em português.
- Todos os identificadores de enum e estruturas de apoio foram convertidos e unificados em inglês.
