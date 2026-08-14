# Relatório de Tradução: src/maps/

## Arquivos Processados
- `interfaces/IMap.h`, `interfaces/MapInteraction.h`
- `utils/MapHelper.h`, `utils/MapHelper.cpp`
- `engine/MapAnimator.h`, `engine/MapAnimator.cpp`
- `engine/MapInputController.h`, `engine/MapInputController.cpp`
- `engine/MapLoader.h`, `engine/MapLoader.cpp`
- `engine/MapPhysics.h`, `engine/MapPhysics.cpp`
- `engine/MapRenderer.h`, `engine/MapRenderer.cpp`
- `engine/MapTileCache.h`, `engine/MapTileCache.cpp`
- `village/Map1Village.h`, `village/Map1Village.cpp`, `village/Map1VillageLayout.h`
- `forest/Map2Forest.h`, `forest/Map2Forest.cpp`, `forest/Map2ForestLayout.h`
- `kingdom/Map3KingdomBridge.h`, `kingdom/Map3KingdomBridge.cpp`, `kingdom/Map3KingdomBridgeLayout.h`
- `kingdom/Map4Kingdom.h`, `kingdom/Map4Kingdom.cpp`, `kingdom/Map4KingdomLayout.h`
- `control/MapController.h`, `control/MapController.cpp`

## Mapeamento de Identificadores (Português -> Inglês)

### Classes e Interfaces
- `IMapa` -> `IMap`
- `InteracaoFloresta` / `ContextoInteracaoFloresta` -> `ForestInteraction` / `ForestInteractionContext`
- `InteracaoVila` / `ContextoInteracaoVila` -> `VillageInteraction` / `VillageInteractionContext`
- `ProximaTransicaoMapa` -> `NextMapTransition`
- `MapHelper`: `checkCollision`, `isWall`, `isEntityOrTeleport`, `tryMove`, `processMovement`
- `MapAnimator`: `animateMapIntroduction`, `animateFlashbang`
- `MapInputController`: `processInputAndCommands`
- `MapLoader`: `enterSubMap`, `standardizeMapSize`
- `MapPhysics`: `applyMapBoundaries`, `moveEnemiesRandomly`
- `MapRenderer`: `renderMap`, `calculateVerticalCamera`, `calculateHorizontalCamera`, `calculateCenteredMargin`
- `MapTileCache`: `hasTile`, `getTile`, `storeTile`, `clear`
- `MapController`: `is3DExplorationActive`, `signal3DMapChange`, `getCurrentMapTitle`

## Observações de Integridade
- Estruturas de layout ASCII dos mapas (vila, caverna, floresta, ponte e castelo) mantidas intactas.
- Diálogos, títulos de mapas e mensagens em tela mantidos estritamente idênticos aos originais em português.
- Compilação realizada com sucesso para todos os arquivos do módulo de mapas.
