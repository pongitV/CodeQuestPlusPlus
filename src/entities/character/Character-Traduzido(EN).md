# Relatorio de Traducao: src/entities/character

## 1. Arquivos Processados
- `Character.h`
- `Character.cpp`

## 2. Mapeamento de Identificadores

### Tipos, Enums e Estruturas:
- `Attributes` (com `health`, `strength`, `dexterity`, `resistance`, `constitution`, `intelligence`, `wisdom`)
- `AttributeType` (`Health`, `Strength`, `Dexterity`, `Resistance`, `Constitution`, `Intelligence`, `Wisdom`)
- `AttackType` (`Single`, `Area`)
- `GameDifficulty` (`Easy`, `Normal`, `Hard`)
- `CombatControl` / `SystemControl` / `AttributeCache` / `DamageCache`

### Metodos de Character:
- `isValid`, `calculateAttributes`, `clone`, `showStatus`, `modifyHealth`, `changeName`, `equipItem`
- `getName`, `getHealth`, `getMaxHealth`, `getStrength`, `getDexterity`, `getResistance`, `getConstitution`, `getIntelligence`, `getWisdom`
- `getLevel`, `getCurrentXp`, `getRequiredXpForLevelUp`, `setLevel`, `setCurrentXp`, `setRequiredXpForLevelUp`, `setHealth`, `gainXp`, `canLevelUp`, `levelUp`
- `scaleAttributes`, `addSoul`, `getSouls`, `getSoulCount`, `removeSoul`
- `forceCacheRecalculation`, `getTotalHealingReceived`, `modifyStaticAttribute`, `getFinalAttributes`
- `getRace`, `getClass`, `getClassName`, `getClassType`, `getRaceType`, `isBoss`
- `getEquippedAt`, `getWeapon`, `getShield`, `getArmor`, `getQuickConsumable`, `unequipConsumable`, `unequipShield`, `unequipWeapon`, `unequipArmor`, `getInventory`, `getItemSelectedForUse`, `setItemSelectedForUse`, `isItemEquipped`
- `isInCombat`, `enterCombat`, `exitCombat`, `prepareForCombat`, `addGold`, `setMultiplier`, `getMultiplier`, `canUseResurrection`, `consumeResurrection`
- `getAbilityCooldown`, `setCooldown`, `getAbilityCanceled`, `setAbilityCanceled`, `setCooldownState`, `getCooldownState`, `setSkipEnemyTurn`, `getSkipEnemyTurn`
- `setReturnToMenu`, `shouldReturnToMenu`, `unlockLabyrinth`, `isLabyrinthUnlocked`, `unlockTrollRegeneration`, `hasTrollRegeneration`, `toggleGodMode`, `isGodMode`, `toggleNoclip`, `isNoclip`
- `reduceCooldowns`, `prepareForNewBattle`, `finishBattle`, `hasEffect`, `getEffectTurns`, `findEffect`
- `setDefending`, `isDefending`, `setDefenseCooldown`, `getDefenseCooldown`, `setAnimatedDeath`, `getAnimatedDeath`, `setParryEnabled`, `isParryEnabled`, `setModernParry`, `isModernParry`, `setAsMinion`, `isMinion`
- `setDifficulty`, `getDifficulty`, `applyDifficultyMultiplier`, `setPlayerIcon`, `getPlayerIcon`, `setPlayerColor`, `getPlayerColor`, `setTerminalBackgroundColor`, `getTerminalBackgroundColor`
- `getAttackType`, `classAbilityConsumesTurn`, `addEffect`, `processTurnStartEffects`, `canAct`, `getActiveEffectIDs`, `clearEffects`, `removeEffect`
- `takeDamage`, `calculateBaseDefense`, `calculateBaseOffensiveDamage`, `ensureMinimumDamage`, `executeDrops`

## 3. Observacoes e Ajustes
- Strings literais mantidas rigorosamente inalteradas.
- Comentarios explicativos atualizados e preservados em portugues.
- Aliases e delegacoes inline preservados para garantir compatibilidade com modulos restantes durante a refatoracao progressiva.
