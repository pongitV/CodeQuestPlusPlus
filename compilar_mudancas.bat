@echo off
:: ============================================================
::  COMPILAR MUDANCAS  (Incremental Build)
::  Recompila apenas os arquivos modificados. Muito mais rapido.
::  Use para o ciclo normal de desenvolvimento.
:: ============================================================
set "MSYS_BIN=C:\msys64\ucrt64\bin"
set "PATH=%MSYS_BIN%;%PATH%"

if not exist build (
    echo AVISO: Pasta "build" nao encontrada.
    echo Execute "compilar_inicio.bat" primeiro.
    pause
    exit /b 1
)

echo.
echo Compilando mudancas...
cmake --build build -j%NUMBER_OF_PROCESSORS%
if %errorlevel% neq 0 (
    echo.
    echo ERRO: Falha ao compilar.
    pause
    exit /b %errorlevel%
)

echo.
echo ============================================================
echo  Compilacao concluida!  bin\CodeQuestPlusPlus.exe
echo ============================================================
echo.
