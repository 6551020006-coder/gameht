@echo off
title Build & Run Game
echo ========================================
echo   Dang build game...
echo ========================================

set W64DEVKIT=C:\raylib\w64devkit
set RAYLIB=C:\raylib\raylib
set GPP=%W64DEVKIT%\bin\g++.exe
set PATH=%W64DEVKIT%\bin;%PATH%

"%GPP%" src\*.cpp ^
    -o game.exe ^
    -I"%RAYLIB%\src" ^
    -L"%W64DEVKIT%\x86_64-w64-mingw32\lib" ^
    -lraylib -lopengl32 -lgdi32 -lwinmm ^
    -std=c++14 -lpthread

if %ERRORLEVEL% == 0 (
    echo [OK] Build thanh cong! Dang khoi dong game...
    echo ========================================
    start "" game.exe
) else (
    echo.
    echo [LOI] Build that bai! Kiem tra code.
    pause
)
