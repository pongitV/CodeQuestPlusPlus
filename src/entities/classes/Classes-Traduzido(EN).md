# Relatorio de Traducao: src/entities/classes

## 1. Arquivos Processados
- `ClassBase.h`
- `ClassFactory.h`
- `ClassFactory.cpp`
- `abilities/Ability.h`
- `archer/Archer.h`
- `archer/Archer.cpp`
- `bard/Bard.h`
- `bard/Bard.cpp`
- `mage/Mage.h`
- `mage/Mage.cpp`
- `necromancer/Necromancer.h`
- `necromancer/Necromancer.cpp`
- `warrior/Warrior.h`
- `warrior/Warrior.cpp`
- `necro-clone/NecroClone.h`
- `necro-clone/NecroClone.cpp`

## 2. Mapeamento de Identificadores

### ClassBase.h:
- `ClassType`: `None`, `Archer`, `Bard`, `Warrior`, `Mage`, `Necromancer`
- `AbilityID`: `None`, `Determination`, `ArcaneChanneling`, `FlashingLights`, `OnSight`, `ThroughTheWire`, `RetreatWithAim`, `SummonSpecter`
- Metodos:
  - `getClassName`
  - `getClassType`
  - `getClassMenuAppearance`
  - `getSpritePath`
  - `getClassAttributes`
  - `getClassEquipment`
  - `getClassPassiveName`
  - `getClassPassiveDescription`
  - `getClassAbilityCooldownDescription`
  - `getClassAbilityName`
  - `getClassAbilityDescription`
  - `useClassAbility`
  - `getAttackType`
  - `abilityConsumesTurn`
  - `notifyCombatMessage`
  - `checkAndReportCooldown`
  - `processBardPassiveHealing`
  - `processBardPassiveBuffMultiplier`
  - `processArcherPassiveArmorPenalty`
  - `applyArcherPassiveSlowPenalty`
  - `revertArcherPassiveSlowPenalty`
  - `executeAttackWithClassPassive`
  - `executeAreaAttack`
  - `executeSingleAttack`
  - `processPreAttackDamage`
  - `processPostAttackDamage`

### Ability.h:
- `getDescription`, `getCooldownTurns`, `consumesTurn`, `use`

### Classes Concretas (Archer, Bard, Mage, Warrior, Necromancer, NecroClone):
- Todos os identificadores, nomes de metodos, variaveis internas e parametros padronizados em ingles.

## 3. Observacoes e Ajustes
- Strings literais mantidas rigorosamente intactas em todo o codigo.
- Comentarios explicativos foram atualizados e preservados em portugues.
- Metodos delegados inline mantidos para compatibilidade retroativa.
