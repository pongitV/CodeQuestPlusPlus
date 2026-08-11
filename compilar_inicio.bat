@echo off
:: ============================================================
::  COMPILAR DO ZERO  (Clean Build)
::  Apaga a pasta "build" e gera tudo do inicio.
::  Use quando: primeira vez, mudancas no CMakeLists.txt, ou
::  problemas estranhos que o incremental nao resolve.
:: ============================================================
set "MSYS_BIN=C:\msys64\ucrt64\bin"
set "PATH=%MSYS_BIN%;%PATH%"

echo.
echo [1/3] Limpando pasta de build...
if exist build rmdir /s /q build
mkdir build

echo.
echo [2/3] Gerando arquivos de build com CMake...
cmake -G "MinGW Makefiles" -S . -B build -DCMAKE_BUILD_TYPE=Debug
if %errorlevel% neq 0 (
    echo ERRO: Falha ao gerar CMake.
    pause
    exit /b %errorlevel%
)

echo.
echo [3/3] Compilando...
cmake --build build -j%NUMBER_OF_PROCESSORS%
if %errorlevel% neq 0 (
    echo.
    echo ERRO: Falha ao compilar.
    pause
    exit /b %errorlevel%
)

echo.
echo ============================================================
echo  Compilacao concluida com sucesso!
echo  Executavel: bin\CodeQuestPlusPlus.exe
echo ============================================================
echo.
pause
