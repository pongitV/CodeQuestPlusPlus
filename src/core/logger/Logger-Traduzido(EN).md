# Relatorio de Traducao: src/core/logger

## 1. Arquivos Processados
- `Logger.h`
- `Logger.cpp`

## 2. Mapeamento de Identificadores
Os identificadores da classe `Logger` ja se encontram padronizados em ingles:
- `Logger::Level` (`Debug`, `Info`, `Warning`, `Error`)
- `log(Level level, const std::string& message, const std::string& source)`
- `logValue(Level level, const std::string& variableName, T value, const std::string& source)`

## 3. Observacoes e Ajustes
- Strings e saidas de console mantidas inalteradas.
