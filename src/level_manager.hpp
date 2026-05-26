#pragma once
#include <raylib.h>

class Game;

class LevelManager {
public:
    static void LoadData(Game& g);
    static void SaveData(Game& g);
    static void StartPhase(Game& g, int level, int phase);
    static void UpdateSpawner(Game& g);
    static void UpdatePowerupSpawner(Game& g);
    static void UpdateWaveLogic(Game& g); // Kiem tra chuyen hiep / qua man
};
