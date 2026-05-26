#pragma once
#include <raylib.h>
#include "laser.hpp"
#include <vector>

enum class EnemyType {
    DRONE,      // Bay ngang, ban thang xuong
    SCOUT,      // Duoi theo player, ban nhanh
    MINI_BOSS,  // Boss nho, mau cam
    BOSS,       // Boss vua, mau do
    GRAND_BOSS, // Dai boss, mau tim, khong lo
    FINAL_BOSS  // Boss cuoi
};

class Enemy {
public:
    Enemy(Vector2 position, EnemyType type, Texture2D tex, Vector2 playerPos = {0, 0}, int bossIdx = 0, Texture2D bulletTex = {0}, Texture2D giantBulletTex = {0});
    ~Enemy();
    void Draw();
    void DrawWarningZone();
    void Update(Vector2 playerPos);
    void FireLaser(Vector2 playerPos);
    Rectangle GetRect() const;

    bool alive;
    bool escapedBottom;  // Thoat ra ngoai man hinh phia duoi
    EnemyType type;
    int hp;
    int scoreValue;
    std::vector<Laser> lasers;
    
    // Boss Attack states (public for Game to manipulate)
    int attackState; 
    float attackTimer;
    float warningTimer;
    float currentAngle;

private:
    Vector2 position;
    Vector2 velocity;
    float speed;
    double lastFireTime;
    float fireInterval;
    int width, height;
    float timeAlive;
    Texture2D image;
    bool hasImage;
    float scale;
    Color tint;

    // Cho 3 state tan cong cua Boss Trung
    int bossIndex;
    Texture2D bossBulletTex;
    Texture2D bossGiantBulletTex;
};
