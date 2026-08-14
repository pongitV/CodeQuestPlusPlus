# Relatorio de Traducao: src/core/utils

## 1. Arquivos Processados
- `Color.h`
- `Constants.h`
- `DialogFunctions.h`
- `DialogFunctions.cpp`
- `EntityPool.h`
- `InputControl.h`
- `InputControl.cpp`
- `InputDispatcher.h`
- `KeybindingManager.h`
- `KeybindingManager.cpp`
- `RandomGenerator.h`
- `RandomGenerator.cpp`
- `RendererProvider.h`
- `RendererProvider.cpp`
- `StringBuffer.h`
- `StringBuffer.cpp`
- `StringConverter.h`

## 2. Mapeamento de Identificadores

### Color.h:
- `RED_CLARO` -> `LIGHT_RED`
- `GREEN_CLARO` -> `LIGHT_GREEN`
- `YELLOW_CLARO` -> `LIGHT_YELLOW`
- `BLUE_CLARO` -> `LIGHT_BLUE`
- `MAGENTA_CLARO` -> `LIGHT_MAGENTA`
- `CYAN_CLARO` -> `LIGHT_CYAN`
- `WHITE_BRILHANTE` -> `BRIGHT_WHITE`
- `FUNDO_BLACK` -> `BG_BLACK`
- `FUNDO_RED` -> `BG_RED`
- `FUNDO_GREEN` -> `BG_GREEN`
- `FUNDO_YELLOW` -> `BG_YELLOW`
- `FUNDO_BLUE` -> `BG_BLUE`
- `FUNDO_MAGENTA` -> `BG_MAGENTA`
- `FUNDO_CYAN` -> `BG_CYAN`
- `FUNDO_WHITE` -> `BG_WHITE`
- `NEGRITO` -> `BOLD`
- `SUBLINHADO` -> `UNDERLINE`
- `PISCANDO` -> `BLINKING`
- `INVERSO` -> `INVERSE`
- `OCULTO` -> `HIDDEN`

### DialogFunctions.h / DialogFunctions.cpp:
- `formatarMsgNarracao` -> `formatNarrationMsg`
- `formatarMsgSistema` -> `formatSystemMsg`
- `formatarMsgHabilidade` -> `formatAbilityMsg`
- `formatarMsgStatus` -> `formatStatusMsg`
- `formatarMsgDrop` -> `formatDropMsg`
- `formatarMsgCombate` -> `formatCombatMsg`
- `formatarMsgInteracao` -> `formatInteractionMsg`
- Parametros: `npcNome` -> `npcName`, `npcCor` -> `npcColor`, `texto` -> `text`, `linhas` -> `lines`, `corTema` -> `themeColor`

### InputControl.h / InputControl.cpp:
- `EstadoInput` -> `InputState`
- `ComandoMapa` -> `MapCommand` (`Cima` -> `Up`, `Baixo` -> `Down`, `Esquerda` -> `Left`, `Direita` -> `Right`, `Ficha` -> `Sheet`, `Nenhum` -> `None`)
- `teclaPressionada` -> `isKeyPressed`
- `lerTecla` -> `readKey`
- `traduzirTeclaParaComando` -> `translateKeyToCommand`
- `limparBuffer` -> `clearBuffer`
- `atualizarTeclas` -> `updateKeys`
- `lerEntradaProtegida` -> `readProtectedInput`
- `lerInteiroComLimites` -> `readIntegerWithBounds`
- `lerSelecaoMenuComSetas` -> `readMenuSelectionWithArrows`
- `lerSelecaoMenuEmPopup` -> `readMenuSelectionInPopup`
- `aguardarEnter` -> `waitForEnter`
- `executarLoopMenuPopup` -> `executePopupMenuLoop`

### InputDispatcher.h:
- `Acao` -> `Action`
- `registrar` -> `registerAction`
- `executar` -> `execute`
- `AcaoComRetorno` -> `ActionWithReturn`
- `PollEntry.tecla` -> `PollEntry.key`
- `PollEntry.acao` -> `PollEntry.action`
- `registrarPoll` -> `registerPoll`
- `limpar` -> `clear`
- `acoes` -> `actions`
- `pollAcoes` -> `pollActions`

### RandomGenerator.h / RandomGenerator.cpp:
- `getInteiro` -> `getInt`
- `rolarChance` -> `rollChance`
- `obterGerador` -> `getGenerator`

### RendererProvider.h / RendererProvider.cpp:
- `instancia` -> `instance`

## 3. Observacoes e Ajustes
- Todas as strings literais permaneceram intactas.
- Comentarios foram mantidos ou traduzidos para o portugues.
- Aliases legados inline foram mantidos para compatibilidade retroativa.
