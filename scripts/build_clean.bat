@echo off
:: ============================================================
::  CLEAN BUILD
::  Deletes the "build" folder and generates everything from scratch.
::  Use when: first time setup, changes in CMakeLists.txt, or
::  resolving unexpected build issues.
:: ============================================================
set "MSYS_BIN=C:\msys64\ucrt64\bin"
set "PATH=%MSYS_BIN%;%PATH%"
set "ROOT_DIR=%~dp0.."

echo.
echo [1/3] Cleaning build directory...
if exist "%ROOT_DIR%\build" rmdir /s /q "%ROOT_DIR%\build"
mkdir "%ROOT_DIR%\build"

echo.
echo [2/3] Generating build files with CMake...
cmake -G "MinGW Makefiles" -S "%ROOT_DIR%" -B "%ROOT_DIR%\build" -DCMAKE_BUILD_TYPE=Debug
if %errorlevel% neq 0 (
    echo ERROR: Failed to generate CMake files.
    pause
    exit /b %errorlevel%
)

echo.
echo [3/3] Compiling...
cmake --build "%ROOT_DIR%\build" -j%NUMBER_OF_PROCESSORS%
if %errorlevel% neq 0 (
    echo.
    echo ERROR: Compilation failed.
    pause
    exit /b %errorlevel%
)

echo.
echo ============================================================
echo  Build completed successfully!
echo  Executable: bin\CodeQuestPlusPlus.exe
echo ============================================================
echo.
pause
