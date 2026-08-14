# Mapeamento de Tradução Linguística: `src/systems/combat/`

## Arquivos Processados
- `src/systems/combat/CombatStatistics.h`
- `src/systems/combat/Parry.h` / `src/systems/combat/Parry.cpp`
- `src/systems/combat/ICombatUI.h`
- `src/systems/combat/CombatUIImpl.h` / `src/systems/combat/CombatUIImpl.cpp`
- `src/systems/combat/CombatRaycasterUIImpl.h` / `src/systems/combat/CombatRaycasterUIImpl.cpp`
- `src/systems/combat/Combat.h` / `src/systems/combat/Combat.cpp`
- `src/systems/combat/mechanics/DamageCalculator.h` / `src/systems/combat/mechanics/DamageCalculator.cpp`
- `src/systems/combat/mechanics/EnemyMechanics.h` / `src/systems/combat/mechanics/EnemyMechanics.cpp`
- `src/systems/combat/mechanics/TurnManager.h` / `src/systems/combat/mechanics/TurnManager.cpp`

---

## Mapeamento de Identificadores (Português -> Inglês)

### Estruturas e Interfaces
- `CombatStatistics` -> `CombatStatistics` (propriedades `totalDamageDealt`, `totalDamageTaken`, etc.)
- `ICombateUI` -> `ICombatUI` (com `using ICombateUI = ICombatUI;`)
- `CombateUIImpl` -> `CombatUIImpl` (com `using CombateUIImpl = CombatUIImpl;`)
- `CombateRaycasterUIImpl` -> `CombatRaycasterUIImpl` (com `using CombateRaycasterUIImpl = CombatRaycasterUIImpl;`)
- `AcaoCombate` -> `CombatAction` (com `using AcaoCombate = CombatAction;`)
- `Combate` / `Combat` -> `Combat`
- `CalculadoraDano` -> `DamageCalculator`
- `MecanicasInimigo` -> `EnemyMechanics`
- `GerenciadorTurnos` -> `TurnManager`

### Métodos de `ICombatUI` / `CombatUIImpl` / `CombatRaycasterUIImpl`
- `configurarContexto3D` -> `configure3DContext`
- `animarIntroducaoCombate` -> `animateCombatIntroduction`
- `atualizarTelaEstatica` -> `updateStaticScreen`
- `animarDanoNoInimigo` -> `animateDamageOnEnemy`
- `animarCuraNoInimigo` -> `animateHealOnEnemy`
- `animarDanoNoJogador` -> `animateDamageOnPlayer`
- `animarCuraNoJogador` -> `animateHealOnPlayer`
- `animarMorteInimigo` -> `animateEnemyDeath`
- `limparContextoPersonagemHUD` -> `clearCharacterHUDContext`
- `limparContextoInimigoMortoEDrops` -> `clearDeadEnemyAndDropsContext`
- `margemCombate` -> `combatMargin`
- `adicionarMensagemFixa` -> `addFixedMessage`
- `limparMensagensFixas` -> `clearFixedMessages`
- `definirMensagemBanner` -> `setBannerMessage`
- `definirTurnoVisivel` -> `setVisibleTurn`
- `obterAcaoDoJogador` -> `getPlayerAction`
- `obterAlvoAtaque` -> `getAttackTarget`
- `obterAlvoItem` -> `getItemTarget`
- `obterEscolhaDeEscudo` -> `getShieldChoice`
- `notificarInimigosMaisAgeis` -> `notifyEnemiesFaster`
- `notificarTurnoExtra` -> `notifyExtraTurn`
- `notificarDesprevencaoInventario` -> `notifyInventoryUnready`
- `notificarSemEscudos` -> `notifyNoShields`
- `notificarDesequilibrioDefesa` -> `notifyDefenseImbalance`
- `notificarPosturaDefensiva` -> `notifyDefensiveStance`
- `notificarAcaoInvalida` -> `notifyInvalidAction`
- `notificarCancelamentoItem` -> `notifyItemCancelled`
- `notificarRequisitoNaoAtendido` -> `notifyRequirementNotMet`
- `displayTelaVitoria` -> `showVictoryScreen`
- `displayTelaDerrota` -> `showDefeatScreen`
- `displayTelaAtributos` -> `showAttributesScreen`
- `displayTelaDiario` -> `showDiaryScreen`
- `limparTela` -> `clearScreen`

### Métodos de `Combat`
- `resetarEstatisticasAvancadas` -> `resetAdvancedStatistics`
- `getNameFormatadoComNumero` -> `getFormattedNameWithNumber`
- `applyDamageAoAlvo` -> `applyDamageToTarget`
- `processarMorteDeInimigo` -> `processEnemyDeath`
- `displayResultadoDoAtaque` -> `displayAttackResult`
- `prepararTurnoPersonagem` -> `prepareCharacterTurn`
- `processarPosDano` -> `processPostDamage`
- `ehPersonagemJogadorOuAliado` -> `isPlayerOrAlly`
- `processarMenuDeAcoesDoJogador` -> `processPlayerActionMenu`
- `processarAcaoAtacar` -> `processAttackAction`
- `processarAcaoDefender` -> `processDefendAction`
- `processarAcaoHabilidade` -> `processAbilityAction`
- `processarAcaoInventario` -> `processInventoryAction`
- `limparInimigosMortos` -> `clearDeadEnemies`
- `selecionarEscudo` -> `selectShield`
- `obterTituloDoCombate` -> `getCombatTitle`
- `obterInimigosRaw` -> `getRawEnemies`
- `displayTelaDeCombate` -> `displayCombatScreen`
- `setContexto3D` -> `set3DContext`
- `obterAliadosVivosRaw` -> `getLivingAlliesRaw`
- `executarTurnoJogadorOuAliado` -> `executePlayerOrAllyTurn`
- `adicionarAliadoEmCombate` -> `addAllyInCombat`
- `adicionarAliados` -> `addLivingAllies`
- `iniciarCombate` -> `startCombat`
- `executarTurnoDeTodosOsInimigos` -> `executeAllEnemiesTurn`
- `verificarCondicaoDeVitoriaOuDerrota` -> `checkWinLossCondition`
- `realizarAtaqueFisico` -> `performPhysicalAttack`
- `obterParriesTentados` -> `getAttemptedParries`
- `obterParriesEfetivos` -> `getEffectiveParries`
- `obterMaiorDanoCausado` -> `getHighestDamageDealt`
- `obterItensConsumidos` -> `getConsumedItems`
- `obterNovasDescobertas` -> `getNewDiscoveries`

---

## Observações
- Todas as strings literais, diálogos e mensagens na tela permaneceram 100% inalteradas.
- Comentários foram mantidos em português.
- Aliases legados inline preservam compatibilidade retroativa para todos os módulos dependentes.
