# Relatorio de Traducao: src/entities/common

## 1. Arquivos Processados
- `DamageResult.h`
- `EquipmentSlot.h`
- `LevelSystem.h`

## 2. Mapeamento de Identificadores

### DamageResult.h:
- `ResultadoDano` -> `DamageResult`
- `danoBloqueado` -> `blockedDamage`
- `escudoQuebrou` -> `shieldBroke`
- `nomeEscudoQuebrado` -> `brokenShieldName`

### EquipmentSlot.h:
- `EquipmentSlot`: `MainHand`, `OffHand`, `Armor`, `Accessory1`, `Consumable` (com aliases legados)

### LevelSystem.h:
- `SistemaDeNivel` -> `LevelSystem`
- `getLevel`, `getCurrentXp`, `getXpToLevelUp`, `setLevel`, `setCurrentXp`, `setXpToLevelUp`, `addXp`, `canLevelUp`

## 3. Observacoes e Ajustes
- Strings literais e compatibilidade retroativa totalmente preservadas.
- Comentarios explicativos mantidos em portugues.
