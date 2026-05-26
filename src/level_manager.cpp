#include "level_manager.hpp"
#include "game.hpp"
#include "boss.hpp"
#include "powerup.hpp"
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <string>

static float RandomX(int enemyWidth) {
    int margin = 50;
    int maxVal = GetScreenWidth() - enemyWidth - margin * 2;
    if (maxVal <= 0) return margin;
    return margin + (float)(rand() % maxVal);
}

// -------------------------------------------------------
// LOAD / SAVE
// -------------------------------------------------------
void LevelManager::LoadData(Game& g) {
    g.highScore = 0;
    g.levels[0] = {true, 0, 0};
    for (int i = 1; i < 3; i++) g.levels[i] = {false, 0, 0};

    std::ifstream in("savegame.dat");
    if (in.is_open()) {
        in >> g.highScore;
        for (int i = 0; i < 3; i++)
            in >> g.levels[i].isUnlocked >> g.levels[i].stars >> g.levels[i].highScore;
        in.close();
    }

    // Reset tien trinh nhung giu diem cao
    g.levels[0].isUnlocked = true;
    g.levels[0].stars = 0;
    for (int i = 1; i < 3; i++) { g.levels[i].isUnlocked = false; g.levels[i].stars = 0; }
}

void LevelManager::SaveData(Game& g) {
    std::ofstream out("savegame.dat");
    if (out.is_open()) {
        out << g.highScore << "\n";
        for (int i = 0; i < 3; i++)
            out << g.levels[i].isUnlocked << " " << g.levels[i].stars << " " << g.levels[i].highScore << "\n";
        out.close();
    }
}

// -------------------------------------------------------
// START PHASE
// -------------------------------------------------------
void LevelManager::StartPhase(Game& g, int level, int phase) {
    g.currentLevel = level;
    g.currentPhase = phase;
    g.spawnedCount = 0;
    g.lastSpawnTime = GetTime();
    g.waveCleared = false;
    g.phaseBossSpawned = false;
    g.bossesKilledInPhase = 0;

    if (phase == 1) { g.enemies.clear(); g.asteroids.clear(); g.powerups.clear(); }

    g.powerupsDropped = 0;
    g.maxPowerupsPerWave = 4 + rand() % 2;
    g.lastPowerupDropTime = GetTime();
    g.nextPowerupInterval = 8.0f + (float)(rand() % 8);
    g.asteroidSpawnTimer = 0.0f;

    if (level == 1) {
        if (phase == 1) { g.spawnQueue=15; g.spawnInterval=1.8f; g.maxOnScreen=4; g.asteroidSpawnInterval=2.5f; g.bossesToKillInPhase=1; }
        else            { g.spawnQueue=15; g.spawnInterval=1.5f; g.maxOnScreen=5; g.asteroidSpawnInterval=1.8f; g.bossesToKillInPhase=1; }
    } else if (level == 2) {
        if (phase == 1) { g.spawnQueue=20; g.spawnInterval=1.5f; g.maxOnScreen=5; g.asteroidSpawnInterval=1.8f; g.bossesToKillInPhase=2; }
        else            { g.spawnQueue=20; g.spawnInterval=1.2f; g.maxOnScreen=6; g.asteroidSpawnInterval=1.5f; g.bossesToKillInPhase=3; }
    } else if (level == 3) {
        if (phase == 1) { g.spawnQueue=25; g.spawnInterval=1.0f; g.maxOnScreen=7; g.asteroidSpawnInterval=1.0f; g.bossesToKillInPhase=3; }
        else            { g.spawnQueue=15; g.spawnInterval=1.0f; g.maxOnScreen=5; g.asteroidSpawnInterval=0.8f; g.bossesToKillInPhase=1; }
    }
}

// -------------------------------------------------------
// UPDATE SPAWNER (quai thuong)
// -------------------------------------------------------
void LevelManager::UpdateSpawner(Game& g) {
    if (g.state != GameState::PLAYING) return;

    if (g.spawnedCount < g.spawnQueue) {
        if ((int)g.enemies.size() >= g.maxOnScreen) return;
        if (GetTime() - g.lastSpawnTime < g.spawnInterval) return;

        int roll = rand() % 100;
        int scoutChance = (g.currentLevel - 1) * 15;
        if (roll < scoutChance)
            g.enemies.push_back(Enemy({RandomX(64),-70}, EnemyType::SCOUT, g.scoutTexture));
        else
            g.enemies.push_back(Enemy({RandomX(64),-70}, EnemyType::DRONE, g.droneTexture));
        g.spawnedCount++;
        g.lastSpawnTime = GetTime();
    } else {
        if (!g.phaseBossSpawned && g.enemies.size() <= 2) {
            float cx = GetScreenWidth() / 2.0f;
            if (g.currentLevel == 1) {
                if (g.currentPhase == 1) {
                    g.enemies.push_back(Enemy({cx-48,-100}, EnemyType::MINI_BOSS, (rand()%2==0)?g.droneTexture:g.scoutTexture));
                } else {
                    int bIdx = rand() % 4;
                    g.enemies.push_back(Enemy({cx-48,-100}, EnemyType::BOSS, g.bossTextures[bIdx].texture, {0,0}, bIdx, g.bossBulletTex[bIdx].texture, g.bossGiantBulletTex[bIdx].texture));
                }
            } else if (g.currentLevel == 2) {
                if (g.currentPhase == 1) {
                    g.enemies.push_back(Enemy({cx-150,-100}, EnemyType::MINI_BOSS, (rand()%2==0)?g.droneTexture:g.scoutTexture));
                    g.enemies.push_back(Enemy({cx+50,-100},  EnemyType::MINI_BOSS, (rand()%2==0)?g.droneTexture:g.scoutTexture));
                } else {
                    g.enemies.push_back(Enemy({cx-48,-100}, EnemyType::MINI_BOSS, (rand()%2==0)?g.droneTexture:g.scoutTexture));
                }
            } else if (g.currentLevel == 3) {
                if (g.currentPhase == 1) {
                    g.enemies.push_back(Enemy({cx-150,-100}, EnemyType::MINI_BOSS, (rand()%2==0)?g.droneTexture:g.scoutTexture));
                    g.enemies.push_back(Enemy({cx+50,-100},  EnemyType::MINI_BOSS, (rand()%2==0)?g.droneTexture:g.scoutTexture));
                } else {
                    g.enemies.push_back(Enemy({cx - g.boss5Tex.texture.width/2.0f, -200}, EnemyType::FINAL_BOSS, g.boss5Tex.texture));
                }
            }
            g.phaseBossSpawned = true;
            PlaySound(g.sfxBoss);
        }
    }
}

// -------------------------------------------------------
// UPDATE POWERUP SPAWNER
// -------------------------------------------------------
void LevelManager::UpdatePowerupSpawner(Game& g) {
    if (g.state != GameState::PLAYING) return;
    if (g.powerupsDropped >= g.maxPowerupsPerWave) return;
    if (GetTime() - g.lastPowerupDropTime < g.nextPowerupInterval) return;

    float x = 32.0f + (float)(rand() % (GetScreenWidth() - 96));
    Vector2 pos = {x, -60.0f};
    int roll = rand() % 4;
    switch (roll) {
        case 0: g.powerups.push_back(Powerup(pos, PowerupType::HEAL,       g.texHeal));      break;
        case 1: g.powerups.push_back(Powerup(pos, PowerupType::RAPID_FIRE, g.texRapidFire)); break;
        case 2: g.powerups.push_back(Powerup(pos, PowerupType::SHIELD,     g.texShield));    break;
        case 3: g.powerups.push_back(Powerup(pos, PowerupType::BOMB,       g.texBomb));      break;
    }
    g.powerupsDropped++;
    g.lastPowerupDropTime = GetTime();
    g.nextPowerupInterval = 8.0f + (float)(rand() % 8);
}

// -------------------------------------------------------
// UPDATE WAVE LOGIC — kiem tra chuyen hiep / qua man
// -------------------------------------------------------
void LevelManager::UpdateWaveLogic(Game& g) {
    if (!g.phaseBossSpawned || g.waveCleared) return;

    // Man 2 Hiep 2: Mini Boss -> 2 Boss
    if (g.currentLevel == 2 && g.currentPhase == 2 && g.bossesKilledInPhase == 1 && g.enemies.empty()) {
        float cx = GetScreenWidth() / 2.0f;
        int bIdx1 = rand() % 4, bIdx2 = rand() % 4;
        g.enemies.push_back(Enemy({cx-200,-100}, EnemyType::BOSS, g.bossTextures[bIdx1].texture, {0,0}, bIdx1, g.bossBulletTex[bIdx1].texture, g.bossGiantBulletTex[bIdx1].texture));
        g.enemies.push_back(Enemy({cx+100,-150}, EnemyType::BOSS, g.bossTextures[bIdx2].texture, {0,0}, bIdx2, g.bossBulletTex[bIdx2].texture, g.bossGiantBulletTex[bIdx2].texture));
        g.bossesKilledInPhase++;
        g.bossesToKillInPhase = 3;
        PlaySound(g.sfxBoss);
    }
    // Man 3 Hiep 1: 2 Mini Boss -> 1 Boss
    else if (g.currentLevel == 3 && g.currentPhase == 1 && g.bossesKilledInPhase == 2 && g.enemies.empty()) {
        float cx = GetScreenWidth() / 2.0f;
        int bIdx3 = rand() % 4;
        g.enemies.push_back(Enemy({cx-48,-100}, EnemyType::BOSS, g.bossTextures[bIdx3].texture, {0,0}, bIdx3, g.bossBulletTex[bIdx3].texture, g.bossGiantBulletTex[bIdx3].texture));
        g.bossesKilledInPhase++;
        g.bossesToKillInPhase = 3;
        PlaySound(g.sfxBoss);
    }

    // Chuyen Hiep hoac Qua Man
    if (g.bossesKilledInPhase >= g.bossesToKillInPhase && g.enemies.empty()) {
        if (g.currentPhase == 1) {
            StartPhase(g, g.currentLevel, 2);
        } else {
            g.waveCleared = true;
            g.victoryPulse = 0.0f;
            g.menuBgY = g.bgY;
            StopSound(g.sfxWin);
            PlaySound(g.sfxWin);
            g.state = GameState::VICTORY;

            int stars = 1;
            if (g.spaceship.hp >= 40) stars = 3;
            else if (g.spaceship.hp >= 20) stars = 2;
            g.levels[g.currentLevel - 1].stars = std::max(g.levels[g.currentLevel - 1].stars, stars);

            if (g.score > g.levels[g.currentLevel - 1].highScore) g.levels[g.currentLevel - 1].highScore = g.score;
            if (g.score > g.highScore) g.highScore = g.score;
            if (g.currentLevel < 3) g.levels[g.currentLevel].isUnlocked = true;
            SaveData(g);
        }
    }
}
