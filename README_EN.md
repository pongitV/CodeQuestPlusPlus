<p align="center">
  <img src="assets/icons/icon.png" alt="CodeQuest++ Direct2D Logo" width="128" height="128" style="border-radius: 24px;" />
</p>

<p align="center">
  <strong>Direct2D port and GPU-accelerated graphics rendering engine for the tactical RPG CodeQuest++ in C++17.</strong>
</p>

<p align="center">
  <a href="https://isocpp.org/"><img src="https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++17" /></a>
  <a href="https://learn.microsoft.com/en-us/windows/win32/direct2d/direct2d-portal"><img src="https://img.shields.io/badge/Graphics-Direct2D%20%7C%20DirectWrite-0078D6?style=for-the-badge&logo=windows&logoColor=white" alt="Direct2D & DirectWrite" /></a>
  <a href="https://cmake.org/"><img src="https://img.shields.io/badge/CMake-3.10+-064F8C?style=for-the-badge&logo=cmake&logoColor=white" alt="CMake" /></a>
  <a href="https://learn.microsoft.com/windows/"><img src="https://img.shields.io/badge/Platform-Windows_10%2F11-0078D6?style=for-the-badge&logo=windows&logoColor=white" alt="Platform Windows" /></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/License-GNU_GPLv3-blue.svg?style=for-the-badge" alt="License GPLv3" /></a>
  <img src="https://img.shields.io/badge/Dependencies-Zero_External-success?style=for-the-badge" alt="Zero External Dependencies" />
</p>

---

<p align="center">
  <strong>Idiomas / Languages:</strong><br>
  <a href="README.md"><strong>Português</strong></a> &nbsp;|&nbsp; <a href="README_EN.md"><strong>English (Current)</strong></a>
</p>

---

## Table of Contents

- [Overview](#overview)
- [Key Features](#key-features)
- [Project Architecture](#project-architecture)
- [Classes, Races, and Systems](#classes-races-and-systems)
- [Game Controls](#game-controls)
- [Building and Running](#building-and-running)
  - [Prerequisites](#prerequisites)
  - [Quick Scripts (Recommended)](#quick-scripts-recommended)
  - [Manual Compilation via CMake](#manual-compilation-via-cmake)
  - [Running the Game](#running-the-game)
- [Graphics Pipeline and Direct2D Rendering](#graphics-pipeline-and-direct2d-rendering)
- [Engineering Retrospective (Terminal to D2D Port)](#engineering-retrospective-terminal-to-d2d-port)
- [License](#license)

---

## Overview

**CodeQuestPlusPlus** is the official graphical port and architectural evolution of the original terminal RPG (**CodeQuestPlusPlus-Terminal**), migrating from a character-based console environment to a native desktop application powered by **GPU-accelerated Direct2D** rendering and **DirectWrite** vector typography in **C++17**.

The project preserves all domain rules, tactical combat mechanics, pseudo-3D (*Raycasting*) exploration, and progression systems from the original game, while completely re-engineering the presentation layer:
- Eliminates framerate bottlenecks and console screen flickering.
- Introduces full high-resolution PNG texture mapping for walls, animated doors, floors, ceilings, and 2D Pixel Art billboard sprites.
- Renders user interfaces (HUD, inventory, dialogues, mini-map, and status screens) with direct hardware acceleration.
- Embeds the native multi-resolution application icon directly into the PE resource table via the Win32 Resource Compiler (`windres`), eliminating the need for administrator privileges to execute.

All of this was accomplished while maintaining the original philosophy of **zero heavy external dependencies** (no SDL, SFML, GLFW, Unreal, or Unity), interacting directly with native Microsoft Windows APIs.

---

## Key Features

- **Textured 3D Raycaster Engine**:
  - Real-time pseudo-3D perspective with textured walls, interactive doors, ceilings, floors, and dynamic distance-based lighting attenuation.
  - Directional 2D Pixel Art billboard sprite rendering for monsters, NPCs, and props projected onto Direct2D bitmap targets.
- **Direct2D & DirectWrite Graphics Pipeline**:
  - Hardware-accelerated presentation layer supporting alpha transparency, flexible layouts, and smooth 60+ FPS animations.
  - Subpixel vector text rendering powered by DirectWrite (`dwrite`), ensuring sharp typography and high legibility.
  - Native asset decoding using Windows Imaging Component (WIC) and GDI+.
- **Turn-Based Tactical Combat**:
  - Dynamic **Parry** mechanism based on reactive timing windows that mitigates or completely nullifies incoming enemy attacks.
  - Class-exclusive active skills, arcane spells, combat inventory, and tactical consumables.
  - Monster AI featuring loot drop tables, dynamic physical/magical stat formulas, and elemental resistances.
- **Comprehensive RPG Systems**:
  - **5 Playable Classes**: Archer, Bard, Mage, Necromancer, Warrior.
  - **5 Races with Unique Passives**: Dwarf, Elf, Human, Orc, NecroClone.
  - Comprehensive inventory management with equipment slots (weapons, armor, shields), consumables, crafting materials, and quest items.
  - Live Bestiary tracking defeated creatures and dynamic real-time Quest Diary.
- **Multi-Map Interconnected World**:
  - **Map 1 (Starting Village)**: Interactive service NPCs (Blacksmith, Alchemist, Food Merchant, Priest).
  - **Map 2 (Dark Forest)**: Labyrinth maze with hostile encounters and hidden treasure chests.
  - **Map 3 (Kingdom Bridge)**: Strategic crossing with patrol guards.
  - **Map 4 (Royal Kingdom Castle)**: Throne room, royal guards, and story progression.
  - Real-time 2D mini-map and full-screen interactive world map.
- **Native Win32 Input (Keyboard + Mouse)**:
  - Low-latency keyboard and mouse event capture directly via the Win32 message loop (`PeekMessage`), without requiring UAC administrator elevation.

---

## Project Architecture

The codebase enforces strict separation of concerns into modular layers:

```
CodeQuestPlusPlus/
├── CMakeLists.txt              # Root CMake configuration
├── build_clean.bat             # Clean build shortcut (root)
├── build_incremental.bat       # Fast incremental recompilation shortcut (root)
├── compilar_inicio.bat         # Portuguese alias for build_clean.bat
├── compilar_mudancas.bat       # Portuguese alias for build_incremental.bat
├── README.md                   # Portuguese Documentation (Direct2D Port)
├── README_EN.md                # English Documentation (Direct2D Port)
├── LICENSE                     # GNU GPLv3 License
├── assets/                     # Visual assets and graphical resources
│   ├── classes/                # Character class sprites in Pixel Art
│   ├── doors/                  # Raycaster door textures
│   ├── floors/                 # Floor textures
│   ├── icons/                  # Application icons (icon.png, icon2.png, icon.ico)
│   ├── races/                  # Character race sprites
│   ├── terrain/                # Ground & environmental textures
│   ├── trees/                  # Tree & foliage billboard sprites
│   ├── walls/                  # Raycaster wall textures
│   └── worldMap.png            # Global world map texture
├── bin/                        # Binary output directory
│   └── CodeQuestPlusPlus.exe   # Compiled Direct2D executable with embedded icon
├── docs/                       # Architecture & technical documentation
├── scripts/                    # Build automation batch scripts (Windows)
│   ├── build_clean.bat         # Full clean build (MSYS2/CMake)
│   ├── build_incremental.bat   # Fast incremental recompilation
│   ├── compilar_inicio.bat     # Clean build (PT)
│   └── compilar_mudancas.bat   # Incremental build (PT)
├── terminal/                   # Reference documentation of the original terminal version
└── src/                        # Main C++17 source code
    ├── main.cpp                # WinMain entry point & primary message loop
    ├── resources/              # Windows resources (resource.rc with embedded icon)
    ├── core/                   # Core engine, Win32 window, D2DContext, Input, Logger
    │   ├── config/             # Game configuration and constants
    │   ├── d2d-context/        # Direct2D and DirectWrite initialization
    │   ├── input/              # Win32 keyboard and mouse input manager
    │   ├── logger/             # Diagnostic logging system
    │   ├── state/              # State machine (StateManager, GameContext)
    │   ├── utils/              # Dialog utilities, string buffers, and keybindings
    │   └── window/             # Win32 GameWindow encapsulation
    ├── entities/               # Game entities and domain rules
    │   ├── character/          # Player character entity and stats
    │   ├── classes/            # Class hierarchy (Archer, Bard, Mage, Necromancer, Warrior)
    │   ├── enemies/            # Enemy archetypes and tactical AI
    │   ├── npcs/               # NPC dialogue logic and interaction handlers
    │   └── races/              # Race hierarchy (Dwarf, Elf, Human, Orc, NecroClone)
    ├── maps/                   # Map layouts, zone logic, and collision maps
    ├── rendering/              # Graphical rendering layer
    │   ├── direct-2d/          # Direct2D pipeline, texture manager, and GridEmulator
    │   └── raycaster/          # 3D Raycaster engine and screen renderers
    ├── systems/                # Core game mechanics
    │   ├── combat/             # Turn-based combat loop and Parry system
    │   ├── inventory/          # Inventory system, equipment, and ItemFactory
    │   └── progress/           # Bestiary, Diary, and quest progression flags
    └── ui/                     # UI screens and graphical presentation adapters
```

---

## Classes, Races, and Systems

### Available Classes
| Class | Specialization | Key Ability |
| :--- | :--- | :--- |
| **Archer** | Agility & Ranged Critical Damage | Piercing multi-shots & evasive maneuvers |
| **Bard** | Tactical Support & Status Manipulation | Healing melodies, morale buffs & stun notes |
| **Mage** | High Burst Elemental Magic | Arcane spells, mana pooling & protective shields |
| **Necromancer** | Life Drain & Shadow Magic | Vital essence siphon & dark summonings |
| **Warrior** | High Defense Tank & Brute Force | Heavy strikes & defensive parry mastery |

### Races
- **Dwarf**: High physical constitution, natural armor bonus, and poison resistance.
- **Elf**: Agility bonus, elevated evasion rating, and innate arcane affinity.
- **Human**: Balanced attributes, adaptable stat scaling, and versatile growth.
- **Orc**: High base strength, physical resilience, and combat rage.
- **NecroClone**: Focus on shadow attributes, high magical affinity, and necrotic resistance.

---

## Game Controls

| Key / Action | Function | Context |
| :---: | :--- | :--- |
| <kbd>W</kbd> / <kbd>↑</kbd> | Move Forward / Advance | Exploration (3D Raycaster) |
| <kbd>S</kbd> / <kbd>↓</kbd> | Move Backward / Retreat | Exploration (3D Raycaster) |
| <kbd>A</kbd> / <kbd>←</kbd> | Turn Left | Exploration (3D Raycaster) |
| <kbd>D</kbd> / <kbd>→</kbd> | Turn Right | Exploration (3D Raycaster) |
| <kbd>E</kbd> | Interact (Doors, Chests, NPCs) | Exploration (3D Raycaster) |
| <kbd>M</kbd> | Toggle 2D Mini-Map | Exploration (3D Raycaster) |
| <kbd>I</kbd> | Open Item Inventory | General |
| <kbd>C</kbd> | View Character Attributes | General |
| <kbd>J</kbd> | Open Quest Diary | General |
| <kbd>B</kbd> | Open Monster Bestiary | General |
| <kbd>ESC</kbd> | Pause Menu / Back | General |
| <kbd>1</kbd> - <kbd>4</kbd> | Select Combat Action (Attack, Magic, Item, Flee) | Turn-Based Combat |
| <kbd>Space</kbd> / <kbd>F</kbd> | Perform **Parry** When Attack Indicator Appears | Turn-Based Combat |
| <kbd>Arrow Keys</kbd> / <kbd>Mouse</kbd> | Navigate Options | Menus / Interactive Screens |
| <kbd>Enter</kbd> / <kbd>Left Click</kbd> | Confirm Selection | Menus / Interactive Screens |

---

## Building and Running

### Prerequisites

1. **Operating System**: Windows 10 or Windows 11 (requires native Direct2D runtime).
2. **C++17 Compiler**: MinGW-w64 (MSYS2 UCRT64 recommended at `C:\msys64\ucrt64\bin`) or compatible GCC/Clang.
3. **CMake**: Version 3.10 or higher installed and available in `PATH`.

### Quick Scripts (Recommended)

The repository provides automated batch scripts for rapid compilation:

- **Clean Build (From Scratch)**:
  ```cmd
  .\build_clean.bat
  ```
  *(or `.\compilar_inicio.bat` or `scripts\build_clean.bat`)*

- **Incremental Build (Fast updates)**:
  ```cmd
  .\build_incremental.bat
  ```
  *(or `.\compilar_mudancas.bat` or `scripts\build_incremental.bat`)*

### Manual Compilation via CMake

To configure and build manually using the command line:

```bash
# 1. Set compiler path (if needed)
set PATH=C:\msys64\ucrt64\bin;%PATH%

# 2. Generate build directory with MinGW Makefiles
cmake -G "MinGW Makefiles" -S . -B build -DCMAKE_BUILD_TYPE=Release

# 3. Build project utilizing all CPU cores
cmake --build build -j
```

### Running the Game

The compiled executable with the embedded application icon is located at `bin/CodeQuestPlusPlus.exe`.

```powershell
.\bin\CodeQuestPlusPlus.exe
```

---

## Graphics Pipeline and Direct2D Rendering

The migration from the console version to Direct2D is built upon three architectural pillars:

1. **Direct2D Hardware Pipeline (`ID2D1HwndRenderTarget`)**:
   - Native render target bound to the Win32 window with hardware double buffering, eliminating screen tearing and synchronizing frame presentation to the monitor refresh rate.
2. **Raycasting Projection on Bitmap Buffers**:
   - The raycasting engine calculates wall slices, Euclidean distance, texture UV mapping, and depth lighting, writing directly into a high-speed bitmap rendered on the GPU.
3. **DirectWrite Typography Engine (`IDWriteFactory`)**:
   - Vector font rendering supporting subpixel ClearType anti-aliasing, providing crisp dialogue boxes, battle logs, and status text.

---

## Engineering Retrospective (Terminal to D2D Port)

Developing this graphical port served as the direct technological evolution of lessons learned from the terminal version:

1. **Overcoming Console Bottlenecks**: The terminal version required CPU string buffer pooling to mitigate tearing. Direct2D offloads rendering to the GPU, guaranteeing a solid, fluid 60+ FPS experience.
2. **Interface Decoupling**: Applying the dependency inversion pattern across UI screens (`IScreenCombatUI`, `IDefeatUI`, `IVictoryUI`) allowed porting the entire game loop without needing to rewrite combat calculations, inventory rules, or progression logic.
3. **Native Assets and PE Resource Packaging**: The game now utilizes high-resolution PNG sprites decoded through WIC/GDI+, alongside a native multi-resolution application icon embedded directly into the PE header table via `resource.rc`.

---

## License

This project is free software licensed under the **GNU General Public License v3.0 (GPLv3)**. For details, see the [LICENSE](LICENSE) file.
