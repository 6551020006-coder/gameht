#pragma once
#include <raylib.h>
#include "boss.hpp"
#include "powerup.hpp"
#include "spaceship.hpp"
#include <string>
#include <vector>

struct LevelData {
    bool isUnlocked;
    int stars;
    int highScore;
};

struct Particle {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float life;
    float size;
};

struct Asteroid {
    Vector2 position;
    float speed;
    float radius;
    float rotation;
    float rotationSpeed;
    bool active;
};

struct AnimTexture {
    Image animImage;
    Texture2D texture;
    int animFrames;
    int currentFrame;
    float frameTimer;
    float frameDuration;
};

struct LightningBeam {
    Vector2 position;
    float width;
    float maxLife;
    float currentLife;
    bool isActive;
    bool isDamaging;
    Color color;
    int type;
    float angle;
    float length;
};

enum class GameState {
    MENU,
    LEVEL_SELECT,
    PLAYING,
    PAUSED,
    GAME_OVER,
    VICTORY,
    INSTRUCTIONS,
    SETTINGS,
    SHIP_SELECT
};

class Game {
public:
    Game();
    ~Game();
    void Draw();
    void Update();
    void HandleInput();
    GameState GetState() const { return state; }

    // -------------------------------------------------------
    // Public helper — dùng bởi UIManager / Manager khác
    // -------------------------------------------------------
    void DrawScrollingBgPublic(Texture2D& tex, float scrollY);

    // -------------------------------------------------------
    // Data — public để các Manager truy cập trực tiếp
    // (đặt ở đây thay vì private để tránh friend class phức tạp)
    // -------------------------------------------------------
    std::vector<Enemy>    enemies;
    std::vector<Powerup>  powerups;
    std::vector<Particle> particles;
    std::vector<Asteroid> asteroids;
    std::vector<LightningBeam> beams;

    Spaceship spaceship;
    float cameraShakeTimer;

    // Textures
    Texture2D droneTexture;
    Texture2D scoutTexture;
    Texture2D octopusTexture;
    AnimTexture bossTextures[4];
    AnimTexture bossBulletTex[4];
    AnimTexture bossGiantBulletTex[4];
    AnimTexture boss5Tex;
    AnimTexture samsetTex;
    AnimTexture finalBossBullets[5];
    Texture2D texAsteroid;
    Texture2D bgTexture;
    Texture2D texStarQuaman;
    Texture2D texShield;
    Texture2D texRapidFire;
    Texture2D texHeal;
    Texture2D texBomb;
    Texture2D shipSelectTextures[3];

    // Level & Phase state
    LevelData levels[3];
    int currentLevel;
    int currentPhase;
    int bossesToKillInPhase;
    int bossesKilledInPhase;
    bool phaseBossSpawned;

    int spawnQueue;
    int spawnedCount;
    double lastSpawnTime;
    float spawnInterval;
    bool waveCleared;
    int maxOnScreen;

    float bgY;
    float asteroidSpawnTimer;
    float asteroidSpawnInterval;

    int score;
    int highScore;

    GameState state;
    float menuPulse;
    float menuBgY;
    float gameOverPulse;
    float victoryPulse;

    bool isGameActive;
    int currentMenuOption;
    int currentPauseOption;
    int currentSettingsOption;
    int currentShipSelectOption;
    GameState stateBeforeSettings;

    bool godMode;

    int powerupsDropped;
    int maxPowerupsPerWave;
    double lastPowerupDropTime;
    float nextPowerupInterval;

    // Audio
    Music bgm[3];
    int   currentBgm;
    bool  bgmPlaying;
    Sound sfxLaser;
    Sound sfxExplode;
    Sound sfxEnemyLaser;
    Sound sfxHit;
    Sound sfxPowerup;
    Sound sfxHurt;
    Sound sfxBoss;
    Sound sfxWin;
    int currentVolume;

    void PlayBGM();
    void StopBGM();
    void UpdateBGM();

private:
    std::vector<Laser> enemyLasers;

    void UpdateFinalBoss(Enemy& boss);
    void DeleteInactiveLasers();
    void UpdateAnimations();
    void CheckCollisions();
    void CheckEnemyLaserCollisions();
    void CheckPowerupCollisions();
    void UseBomb();
};
