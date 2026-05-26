#pragma once
#include <raylib.h>

class Game;

class UIManager {
public:
    static void DrawMenu(Game& g);
    static void DrawLevelSelect(Game& g);
    static void DrawInstructions(Game& g);
    static void DrawSettings(Game& g);
    static void DrawGameOver(Game& g);
    static void DrawVictory(Game& g);
    static void DrawPaused(Game& g);
    static void DrawShipSelect(Game& g);
    static void DrawHUD(Game& g);
    static void DrawHealthBar(Game& g, const class Enemy& enemy);
};
