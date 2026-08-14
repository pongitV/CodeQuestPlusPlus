# Relatorio de Traducao: src/core/state

## 1. Arquivos Processados
- `Debug.h`
- `Debug.cpp`
- `Drops.h`
- `Drops.cpp`
- `EnemyCreator.h`
- `EnemyCreator.cpp`
- `GameMenu.h`
- `GameMenu.cpp`
- `MapManager.h`
- `MapManager.cpp`
- `StateManager.h`
- `StateManager.cpp`
- `Status.h`
- `Status.cpp`
- `Store.h`
- `Store.cpp`

## 2. Mapeamento de Identificadores

### Debug:
- `displayDebugMenu` -> `showDebugMenu`
- `isDebugKey(char tecla)` -> `isDebugKey(char key)`
- Variaveis: `mensagemFeedback` -> `feedbackMessage`, `escolha` -> `choice`, `opcoes` -> `options`, etc.

### Drops:
- `reportAndProcessXPGold(Character* jogador, ...)` -> `reportAndProcessXPGold(Character* player, ...)`
- `reportItemDrop(const std::string& nomeItem, ...)` -> `reportItemDrop(const std::string& itemName, ...)`
- `giveAndProcessItem(Character* jogador, ..., std::vector<std::string>& itensObtidos, int chanceDeDrop)` -> `giveAndProcessItem(Character* player, ..., std::vector<std::string>& obtainedItems, int dropChance)`

### EnemyCreator:
- Template: `RacaType` -> `RaceType`, `ClasseType` -> `ClassType`
- Variaveis: `horda` -> `horde`, `variacaoVida` -> `healthVariation`, `variacaoForca` -> `strengthVariation`, `variacaoDestreza` -> `dexterityVariation`

### MapManager:
- `MapType::Labirinto` -> `MapType::Labyrinth`
- `mapaCache` -> `mapCache`
- `matriz` -> `matrix`

### StateManager:
- `ContextoDoJogo` -> `GameContext` (`objetoJogador` -> `playerEntity`, `janela` -> `window`, `renderizador` -> `renderer`, `grade` -> `grid`)
- `EstadoDoJogo` -> `GameState` (`aoEntrar` -> `onEnter`, `executar` -> `execute`, `aoSair` -> `onExit`)
- `Jogo` -> `Game` (`estadoAtual` -> `currentState`, `proximoEstado` -> `nextState`, `mudancaPendente` -> `pendingChange`, `contexto` -> `context`, `mudarEstado` -> `changeState`, `executarLoop` / `rodar` -> `runLoop` / `run`)
- `EstadoExploracao` -> `ExplorationState`
- `EstadoMenu` -> `MenuState`

### Status:
- Classes:
  - `EfeitoStatus` -> `StatusEffect`
  - `EfeitoAtordoamento` -> `StunEffect`
  - `EfeitoSugaSangue` -> `LifeStealEffect`
  - `EfeitoLentidao` -> `SlowEffect`
  - `EfeitoFraqueza` -> `WeaknessEffect`
  - `EfeitoQuebraResistencia` -> `ArmorBreakEffect`
  - `EfeitoSangramento` -> `BleedingEffect`
  - `EfeitoNecrose` -> `NecrosisEffect`
  - `EfeitoMetadeDano` -> `HalfDamageEffect`
  - `EfeitoBuffAtributos` -> `AttributeBuffEffect`
  - `EfeitoInviolavel` -> `InviolableEffect`
  - `EfeitoMiraCerteira` -> `TrueAimEffect`
  - `EfeitoGritoGuerra` -> `WarCryEffect`
  - `EfeitoRodaAdaptacao` -> `AdaptationWheelEffect`
- Metodos de BaseStatus: `onEnter`, `applyTurnStart`, `onExit`, `processIncomingDamage`, `preventsAction`
- Variaveis membro traduzidas para ingles (ex: `lostStrength`, `lostResistance`, `damagePerTurn`, `bonusStrength`, etc.).

### Store:
- Variaveis internas em ingles: `sortedItems`, `product`, `buyAmount`, `maxBuyer`, `maxPossible`, etc.

## 3. Observacoes e Ajustes
- Nenhuma string literal de interface, mensagens de dialogos ou efeitos foi alterada.
- Comentarios foram mantidos ou convertidos para o portugues.
- Aliases de compatibilidade legada foram incluidos para garantir continuidade do build.
