# Relatorio de Traducao: src/entities/races

## 1. Arquivos Processados
- `RaceBase.h`
- `RaceFactory.h`
- `RaceFactory.cpp`
- `dwarf/Dwarf.h`
- `dwarf/Dwarf.cpp`
- `elf/Elf.h`
- `elf/Elf.cpp`
- `human/Human.h`
- `human/Human.cpp`
- `orc/Orc.h`
- `orc/Orc.cpp`
- `necro-clone/NecroClone.h`
- `necro-clone/NecroClone.cpp`

## 2. Mapeamento de Identificadores

### RaceBase.h:
- `RaceType`: `None`, `Dwarf`, `Elf`, `Human`, `Orc`, `ExiledOrc`, `Goblin`, `Fairy`, `Slime`, `ForestAbomination`, `Mimic`, `Troll`, `Mahoraga`
- Metodos:
  - `getRaceType`
  - `getBestiaryInfo`
  - `onPerfectParrySuffered`
  - `ignoresParry`
  - `ignoresShield`
  - `dropLoot`
  - `onDamageDealt`
  - `tryUseActiveAbility`

### RaceFactory:
- `FabricaRacas` -> `RaceFactory`
- `createRace`, `getPlayableRaces`

### Dwarf, Elf, Human, Orc:
- Classes e metodos padronizados: `getRaceName`, `getRaceType`, `getRaceSpritePath`, `getRaceAppearance`, `getRaceAttributes`, `getRaceAbilityName`, `getRaceAbilityDescription`, `processDefensiveDamage`, `processOffensiveDamage`.
- `Ork` -> `Orc` (com alias `using Ork = Orc;`)

## 3. Observacoes e Ajustes
- Strings literais permanecem exatamente identicas.
- Comentarios explicativos foram mantidos e atualizados para portugues.
- Aliases de compatibilidade legada foram adicionados.
