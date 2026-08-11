# CodeQuestPlusPlus (Motor Direct2D)

Um RPG modular em C++17 apresentando renderização **Direct2D** acelerada por hardware ao lado de um motor 3D Raycaster personalizado. Projetado com APIs nativas Win32, sem a sobrecarga de frameworks de terceiros, com padrões de design limpos e arquitetura estritamente padronizada.

Ponto de entrada principal: [src/main.cpp](src/main.cpp)

---

## Sistema de Build (MinGW-w64 & CMake)

O repositório fornece scripts `.bat` automatizados para inicialização rápida do ambiente e compilação incremental com MinGW-w64 (UCRT64).

### Pré-requisitos
- **MinGW-w64** (MSYS2 UCRT64 recomendado, instalado em `C:\msys64\ucrt64\bin`)
- **CMake** (3.10 ou superior)

### Compilação Limpa (Do Zero)
Gera o ambiente de build e compila todos os alvos:
```bat
.\compilar_inicio.bat
```

### Compilação Incremental (Mudanças no Código)
Recompilação rápida para o desenvolvimento do dia a dia:
```bat
.\compilar_mudancas.bat
```

---

## Estrutura do Projeto

```
CodeQuestPlusPlus/
├── CMakeLists.txt
├── compilar_inicio.bat
├── compilar_mudancas.bat
├── README.md
├── resource.rc
├── src/
│   ├── main.cpp
│   ├── core/               # Inicialização da Engine, janela Win32, gerenciamento de estado, Input & Logger
│   │   ├── config/
│   │   ├── d2d-context/
│   │   ├── input/
│   │   ├── logger/
│   │   ├── state/          # StateManager, GameContext, GameStates
│   │   ├── utils/
│   │   └── window/         # Wrapper Win32 da GameWindow
│   ├── entities/           # Entidades do jogo, modelo do Personagem, hierarquias de Raça e Classe, NPCs
│   │   ├── character/
│   │   ├── classes/
│   │   ├── common/
│   │   ├── enemies/
│   │   ├── npcs/
│   │   └── races/
│   ├── maps/               # Zonas do mundo, interações com ambiente, layouts e estado do mapa
│   ├── rendering/          # Pipeline Direct2D e Motor 3D Raycaster
│   │   ├── direct-2d/      # D2DRenderer, FramePipeline, GridEmulator2D
│   │   └── raycaster/      # Núcleo do Raycaster, gerenciadores de textura, renderizadores de cena 3D
│   ├── systems/            # Mecânicas centrais do jogo (Combate, Progresso, Inventário)
│   └── ui/                 # Adaptadores de tela UI, interfaces, componentes e gerenciadores de layout
```

---

## Arquitetura & Pipeline de Renderização

* **Renderizador de Hardware Direct2D:** Loop de eventos Win32 puro (`PeekMessage`) com aceleração de hardware Direct2D (`ID2D1HwndRenderTarget`, `IDWriteFactory`).
* **Visão 3D Raycaster:** Raycasting rápido via software renderizado em buffers de bitmap de hardware.
* **Camada de UI Desacoplada:** Princípio de inversão de dependência aplicado em todas as telas por meio de interfaces abstratas (`IDefeatUI`, `IVictoryUI`, `ICombatUI`, etc.).
* **Máquina de Estados:** Máquina de estados finitos limpa (`StateManager`, `ExplorationState`, `MenuState`) para navegação perfeita.

---

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.
