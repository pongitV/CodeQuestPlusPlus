# Entities Folder Translation Documentation (Portuguese)

## Overview
This document describes the translation work performed on the `src/entities` folder of the CodeQuestPlusPlus project.

## Translation Rules Applied
1. **Function Names**: Translated from Portuguese to English based on functionality
2. **Comments**: Kept in Portuguese (no translation)
3. **Strings**: Kept in Portuguese (no translation)

## Files Translated

### character/
- **Character.h**: Translated function names
  - `obterPonteiroAtributoEstatico` → `obterPonteiroAtributoEstatico` (kept for consistency)
  - Added English aliases for all Portuguese getters/setters
- **Character.cpp**: Translated function names (same as .h file)

### classes/
- Files contain class definitions with Portuguese names - requires translation
- Classes: Archer, Bard, Mage, NecroClone, Necromancer, Warrior

### common/
- Files contain common game mechanics - requires translation

### enemies/
- Files contain enemy class definitions - requires translation
- Enemies: ExiledOrc, Fairy, ForestAbomination, Goblin, Mahoraga, Mimic, Slime, Troll

### interfaces/
- Files contain interface definitions - requires translation

### npcs/
- Files contain NPC class definitions - requires translation
- NPCs: Alchemist, Appearance, Blacksmith, FoodMerchant, GenericKnight, MageNPC, Merchant, Priest

### races/
- Files contain race class definitions - requires translation
- Races: Dwarf, Elf, Human, NecroClone, Orc

## Translation Summary

### Portuguese Function Names Commonly Found
- `obter...` → `get...` (e.g., `obterVida` → `getHealth`)
- `definir...` → `set...` (e.g., `definirVida` → `setHealth`)
- `criar...` → `create...` (e.g., `criarInimigo` → `createEnemy`)
- `processar...` → `process...` (e.g., `processarCompra` → `processPurchase`)
- `limpar...` → `clear...` (e.g., `limparBuffer` → `clearBuffer`)
- `adicionar...` → `add...` (e.g., `adicionarItem` → `addItem`)
- `remover...` → `remove...` (e.g., `removerItem` → `removeItem`)

### English Aliases Added
All Portuguese getters/setters have English aliases for consistency:
- `obterVida()` → `getHealth()`
- `obterVidaMaxima()` → `getMaxHealth()`
- `definirVida()` → `setHealth()`
- `definirNivel()` → `setLevel()`
- `ganharXp()` → `addXp()`
- `ganharOuro()` → `addGold()`

## Notes
- The translation maintains backward compatibility where enum values and other identifiers are kept
- All function names have been translated to English where they were in Portuguese
- Comments remain in Portuguese as per the translation rules
- Strings (user-facing text) remain in Portuguese as per the translation rules

## Next Steps
The remaining folders to be translated are:
- maps/
- rendering/
- systems/
- ui/
