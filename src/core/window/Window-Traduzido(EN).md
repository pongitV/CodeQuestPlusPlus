# Relatorio de Traducao: src/core/window

## 1. Arquivos Processados
- `GameWindow.h`
- `GameWindow.cpp`

## 2. Mapeamento de Identificadores

### Metodos e Funcoes:
- `obterHWND` -> `getHWND`
- `obterHInstance` -> `getHInstance`
- `obterLargura` -> `getWidth`
- `obterAltura` -> `getHeight`
- `processarMensagens` -> `processMessages`
- `teclaPressionada` -> `isKeyPressed`
- `limparTeclas` -> `clearKeys`
- `obterMouseX` -> `getMouseX`
- `obterMouseY` -> `getMouseY`
- `mouseClicado` -> `isMouseClicked`
- `limparMouse` -> `clearMouse`
- `ocultarCursor` -> `hideCursor`
- `mostrarCursor` -> `showCursor`
- `isCursorOculto` -> `isCursorHidden`
- `resolverCaminhoAsset` -> `resolveAssetPath`
- `carregarIconeDePNG` -> `loadIconFromPNG`

### Variaveis Membro e Estaticas:
- `m_largura` -> `m_width`
- `m_altura` -> `m_height`
- `s_teclas` -> `s_keys`
- `s_mouseClicado` -> `s_mouseClicked`
- `s_cursorOculto` -> `s_cursorHidden`

## 3. Observacoes e Ajustes
- Foram mantidos aliases inline para permitir transicao incremental sem quebra de compilacao.
- Strings literais de registro de classe Win32 e caminhos de assets foram preservados integralmente.
