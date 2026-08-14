# Relatório de Tradução: src/entities/enemies/

## Arquivos Processados
- `ClassBaseEnemy.h` e `ClassBaseEnemy.cpp`
- `goblin/Goblin.h` e `goblin/Goblin.cpp`
- `slime/Slime.h` e `slime/Slime.cpp`
- `fairy/Fairy.h` e `fairy/Fairy.cpp`
- `exiled-orc/ExiledOrc.h` e `exiled-orc/ExiledOrc.cpp`
- `forest-abomination/ForestAbomination.h` e `forest-abomination/ForestAbomination.cpp`
- `troll/Troll.h` e `troll/Troll.cpp`
- `mimic/Mimic.h` e `mimic/Mimic.cpp`
- `mahoraga/Mahoraga.h` e `mahoraga/Mahoraga.cpp`

## Mapeamento de Identificadores (Português -> Inglês)

### Classes e Tipos
- `ClassBaseInimigo` -> `ClassBaseEnemy` (com alias `using ClassBaseInimigo = ClassBaseEnemy;`)
- `OrkExilado` -> `ExiledOrc` (com alias `using OrkExilado = ExiledOrc;`)
- `AbominacaoFloresta` -> `ForestAbomination` (com alias `using AbominacaoFloresta = ForestAbomination;`)
- `RaceType::Goblin`, `RaceType::Slime`, `RaceType::Fairy`, `RaceType::ExiledOrc`, `RaceType::ForestAbomination`, `RaceType::Troll`, `RaceType::Mimic`, `RaceType::Mahoraga`

### Métodos e Membros
- `obterRaceType()` -> `getRaceType()`
- `obterBestiaryInfo()` -> `getBestiaryInfo()`
- `realizarDrops(...)` -> `performDrops(...)`
- `aoCausarDano(...)` -> `onDealingDamage(...)`
- `aoSofrerParryPerfeito()` -> `onSufferingPerfectParry()`
- `aoTerAtaqueBloqueadoPorEscudo()` -> `onAttackBlockedByShield()`
- `ignoraParry()` -> `ignoresParry()`
- `ignoraEscudo()` -> `ignoresShield()`
- `curandoAtivamente` -> `activelyHealing`
- `ouroRoubadoTotal` -> `totalStolenGold`
- `parrysSofridos` -> `sufferedParries`
- `defesasComEscudoSofridas` -> `sufferedShieldDefenses`

## Observações de Integridade
- Todas as strings literais, diálogos e descrições do Bestiário foram rigorosamente preservadas sem qualquer alteração.
- Comentários foram mantidos em português.
- Métodos e tipos legados mantidos como delegates e aliases para compatibilidade com o restante da base de código.
