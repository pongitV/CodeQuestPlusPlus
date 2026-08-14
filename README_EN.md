# CodeQuestPlusPlus (Direct2D Engine)

A modular C++17 RPG featuring hardware-accelerated **Direct2D** rendering alongside a custom 3D Raycaster engine. Designed with native Win32 APIs, without third-party framework overhead, strictly following clean design patterns and standardized architectures.

Main entrypoint: [src/main.cpp](src/main.cpp)

---

## Build System (MinGW-w64 & CMake)

The repository provides automated `.bat` scripts for quick environment initialization and incremental compilation with MinGW-w64 (UCRT64).

### Prerequisites
- **MinGW-w64** (MSYS2 UCRT64 recommended, installed in `C:\msys64\ucrt64\bin`)
- **CMake** (3.10 or higher)

### Clean Build (From Scratch)
Generates the build environment and compiles all targets:
```bat
.\build_clean.bat
```
*(or `.\compilar_inicio.bat`)*

### Incremental Build (Code Changes)
Fast recompilation for day-to-day development:
```bat
.\build_incremental.bat
```
*(or `.\compilar_mudancas.bat`)*

---

## Project Structure

```
CodeQuestPlusPlus/
├── CMakeLists.txt
├── build_clean.bat / compilar_inicio.bat
├── build_incremental.bat / compilar_mudancas.bat
├── README.md               # Portuguese Documentation
├── README_EN.md            # English Documentation
├── resource.rc
├── docs/                   # Translation & Architecture Documentation
└── src/
    ├── main.cpp            # Main WinMain Entrypoint & lifecycle loop
    ├── core/               # Engine initialization, Win32 window, State management, Input & Logger
    │   ├── config/
    │   ├── d2d-context/
    │   ├── input/
    │   ├── logger/
    │   ├── state/          # StateManager, GameContext, GameStates
    │   ├── utils/
    │   └── window/         # Win32 GameWindow wrapper
    ├── entities/           # Game entities, Character model, Race & Class hierarchies, NPCs
    │   ├── character/      # Character entity, stats, attributes
    │   ├── classes/        # Archer, Bard, Mage, Necromancer, Warrior
    │   ├── common/         # Enums, LevelSystem, EquipmentSlot
    │   ├── enemies/        # Enemy AI & monster archetypes
    │   ├── interfaces/     # IAttacker, IDamageable
    │   ├── npcs/           # Merchant, Blacksmith, Alchemist, Priest
    │   └── races/          # Dwarf, Elf, Human, Orc, NecroClone
    ├── maps/               # World zones, environmental interactions, map layouts & state
    ├── rendering/          # Direct2D pipeline & 3D Raycaster Engine
    │   ├── config/         # Rendering constants & buffer config
    │   ├── direct-2d/      # D2DRenderer, FramePipeline, GridEmulator2D
    │   └── raycaster/      # Raycaster core, texture managers, 3D scene renderers
    ├── systems/            # Core game mechanics (Combat, Progress, Inventory)
    │   ├── combat/         # Turn-based combat engine, Parry mechanics, damage calculation
    │   ├── inventory/      # Inventory system, Item hierarchies, ItemFactory
    │   └── progress/       # Progression, Diary, Bestiary, Flags
    └── ui/                 # UI screen adapters, interfaces, components, layout managers
```

---

## Architecture & Rendering Pipeline

* **Direct2D Hardware Renderer:** Pure Win32 event loop (`PeekMessage`) with Direct2D hardware acceleration (`ID2D1HwndRenderTarget`, `IDWriteFactory`).
* **3D Raycaster View:** Fast software raycasting rendered into hardware bitmap buffers.
* **Decoupled UI Layer:** Dependency inversion principle applied across all screens through abstract interfaces (`IDefeatUI`, `IVictoryUI`, `IScreenCombatUI`, etc.).
* **State Machine:** Clean finite state machine (`StateManager`, `ExplorationState`, `MenuState`) for seamless navigation.

---

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.
