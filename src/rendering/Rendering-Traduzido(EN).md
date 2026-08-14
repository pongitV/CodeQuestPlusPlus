# Relatório de Tradução: src/rendering

## 1. Arquivos Processados
- `config/RenderingConfig.h`
- `direct-2d/D2DRenderer.h` / `D2DRenderer.cpp`
- `direct-2d/FramePipeline.h` / `FramePipeline.cpp`
- `direct-2d/GridEmulator2D.h` / `GridEmulator2D.cpp`
- `direct-2d/UIRenderer2D.h` / `UIRenderer2D.cpp`
- `raycaster/ScreenManager.h` / `ScreenManager.cpp`
- `raycaster/engine-raycaster/Illuminator.h`
- `raycaster/engine-raycaster/MapCache.h`
- `raycaster/engine-raycaster/Raycaster.h` / `Raycaster.cpp`
- `raycaster/engine-raycaster/RaycasterControls.h` / `RaycasterControls.cpp`
- `raycaster/engine-raycaster/RaycasterEnemies.h` / `RaycasterEnemies.cpp`
- `raycaster/engine-raycaster/RaycasterFrame.h`
- `raycaster/engine-raycaster/RaycasterHUD.h` / `RaycasterHUD.cpp`
- `raycaster/engine-raycaster/RaycasterNPCs.h` / `RaycasterNPCs.cpp`
- `raycaster/engine-raycaster/RaycasterRenderer.h` / `RaycasterRenderer.cpp`
- `raycaster/engine-raycaster/RaycasterRendererCombat.h` / `RaycasterRendererCombat.cpp`
- `raycaster/engine-raycaster/RaycasterRendererImpl.h`
- `raycaster/engine-raycaster/RaycasterSprites.h` / `RaycasterSprites.cpp`
- `raycaster/engine-raycaster/RaycasterWorld.h` / `RaycasterWorld.cpp`
- `raycaster/engine-raycaster/SkyRenderer.h` / `SkyRenderer.cpp`
- `raycaster/engine-raycaster/TextureManager.h` / `TextureManager.cpp`
- `raycaster/screens/attributes/ScreenAttributesRaycaster.h` / `ScreenAttributesRaycaster.cpp`
- `raycaster/screens/bestiary/ScreenBestiaryRaycaster.h` / `ScreenBestiaryRaycaster.cpp`
- `raycaster/screens/combat/ScreenCombatRaycaster.h` / `ScreenCombatRaycaster.cpp`
- `raycaster/screens/defeat/ScreenDefeatRaycaster.h` / `ScreenDefeatRaycaster.cpp`
- `raycaster/screens/diary/ScreenDiaryRaycaster.h` / `ScreenDiaryRaycaster.cpp`
- `raycaster/screens/inventory/ScreenInventoryRaycaster.h` / `ScreenInventoryRaycaster.cpp`
- `raycaster/screens/menu/ScreenClassRaycaster.cpp`
- `raycaster/screens/menu/ScreenDifficultyRaycaster.cpp`
- `raycaster/screens/menu/ScreenIntroductionRaycaster.cpp`
- `raycaster/screens/menu/ScreenMenuRaycaster.cpp`
- `raycaster/screens/menu/ScreenNameRaycaster.cpp`
- `raycaster/screens/menu/ScreenParryRaycaster.cpp`
- `raycaster/screens/menu/ScreenRaceRaycaster.cpp`
- `raycaster/screens/pause/ScreenPauseRaycaster.h` / `ScreenPauseRaycaster.cpp`
- `raycaster/screens/victory/ScreenVictoryRaycaster.h` / `ScreenVictoryRaycaster.cpp`
- `raycaster/screens/utils/MenuRaycasterUtils.h` / `MenuRaycasterUtils.cpp`
- `raycaster/screens/utils/MenuD2DUtils.h` / `MenuD2DUtils.cpp`

## 2. Mapeamento de Identificadores

### Direct2D:
- `D2DRenderer`:
  - `redimensionar` -> `resize`
  - `comecarQuadro` -> `beginFrame`
  - `finalizarQuadro` -> `endFrame`
  - `limpar` -> `clear`
  - `obterRenderTarget` -> `getRenderTarget`
  - `obterDWriteFactory` -> `getDWriteFactory`
  - `obterFontePadrao` -> `getDefaultFont`
  - `obterTexturaBackbuffer` -> `getBackbufferTexture`
- `GridEmulator2D`:
  - `definirCelulaTamanho` -> `setCellSize`
  - `definirGridDimensoes` -> `setGridDimensions`
  - `obterColunas` -> `getColumns`
  - `obterLinhas` -> `getRows`
  - `obterCelulaTamanho` -> `getCellSize`
  - `obterLarguraTotal` -> `getTotalWidth`
  - `obterAlturaTotal` -> `getTotalHeight`
  - `renderizarGrid` -> `renderGrid`

### Engine Raycaster:
- `Raycaster`:
  - `iniciarExploracao3D` -> `start3DExploration`
  - `desenharQuadroEstatico3D` -> `drawStatic3DFrame`
  - `renderizarFundoAtualizadoD2D` -> `renderUpdatedD2DBackground`
- `RaycasterControls`:
  - `processarInputEControles` -> `processInputAndControls`
- `RaycasterWorld`:
  - `obterMapHash` -> `getMapHash`
  - `obterNPCProximo` -> `getNearbyNPC`
  - `obterTemaCeu` -> `getSkyTheme`
  - `isTemaFloresta` -> `isForestTheme`
- `RaycasterRendererCombate`:
  - `RaycasterRendererCombate` -> `RaycasterRendererCombat` (com alias `using RaycasterRendererCombate = RaycasterRendererCombat`)

### Raycaster Screens:
- `TelaAtributosRaycaster`: `AttributesRaycasterScreen`, `EffectID` e `GameDifficulty` alinhados.
- `TelaBestiarioRaycaster`: `BestiaryRaycasterScreen`, campos `name`, `passiveAbility`, `attributesText` padronizados.
- `TelaCombateRaycaster`: `CombatRaycasterScreen`.
- `TelaDerrotaRaycaster`: `DefeatRaycasterScreen`.
- `TelaDiarioRaycaster`: `DiaryRaycasterScreen`.
- `TelaInventarioRaycaster`: `InventoryRaycasterScreen`.
- `TelaPauseRaycaster`: `PauseRaycasterScreen`.
- `TelaVitoriaRaycaster`: `VictoryRaycasterScreen`.
- `GerenciadorTelasRaycaster`: `RaycasterScreenManager`.

## 3. Observações e Ajustes
- Strings literais e logs visuais foram preservados na íntegra.
- Comentários foram mantidos em português.
- Enums de sistema (`GameDifficulty`, `EffectID`, `EquipmentType`) foram devidamente harmonizados.
