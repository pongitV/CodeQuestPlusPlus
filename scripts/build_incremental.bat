@echo off
:: ============================================================
::  INCREMENTAL BUILD
::  Recompiles only modified files. Much faster.
::  Use for daily development cycle.
:: ============================================================
set "MSYS_BIN=C:\msys64\ucrt64\bin"
set "PATH=%MSYS_BIN%;%PATH%"
set "ROOT_DIR=%~dp0.."

if not exist "%ROOT_DIR%\build" (
    echo WARNING: "build" directory not found.
    echo Please run "build_clean.bat" or "compilar_inicio.bat" first.
    pause
    exit /b 1
)

echo.
echo Compiling changes...
cmake --build "%ROOT_DIR%\build" -j%NUMBER_OF_PROCESSORS%
if %errorlevel% neq 0 (
    echo.
    echo ERROR: Compilation failed.
    pause
    exit /b %errorlevel%
)

echo.
echo ============================================================
echo  Build completed!  bin\CodeQuestPlusPlus.exe
echo ============================================================
echo.
