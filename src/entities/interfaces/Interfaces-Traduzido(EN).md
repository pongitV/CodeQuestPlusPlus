# Relatorio de Traducao: src/entities/interfaces

## 1. Arquivos Processados
- `IAttacker.h`
- `IDamageable.h`

## 2. Mapeamento de Identificadores

### IAttacker.h:
- `calcularDanoOfensivoBase` -> `calculateBaseOffensiveDamage`
- `garantirDanoMinimo` -> `ensureMinimumDamage`

### IDamageable.h:
- `receberDano` -> `takeDamage`
- `calcularDefesaBase` -> `calculateBaseDefense`
- Parametros: `danoBruto` -> `rawDamage`, `danoPerfurante` -> `piercingDamage`, `danoReduzidoParry` -> `parryReducedDamage`, `atacante` -> `attacker`, `aplicarPassivas` -> `applyPassives`

## 3. Observacoes e Ajustes
- Nao ha strings literais nestes arquivos.
- Comentarios foram mantidos em portugues.
- Metodos legados inline foram mantidos para compatibilidade retroativa.
