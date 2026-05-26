#include "boss.hpp"
#include <cmath>
#include <cstdlib>
#include <algorithm>

Enemy::Enemy(Vector2 position, EnemyType type, Texture2D tex, Vector2 playerPos, int bossIdx, Texture2D bulletTex, Texture2D giantBulletTex)
    : alive(true), escapedBottom(false), type(type), hp(0), scoreValue(0),
      position(position), velocity({0, 0}), speed(0),
      lastFireTime(0.0), fireInterval(0), width(0), height(0),
      timeAlive(0.0), hasImage(false), scale(1.0f), tint(WHITE),
      bossIndex(bossIdx), attackState(0), attackTimer(0.0f), warningTimer(0.0f),
      currentAngle(0.0f), bossBulletTex(bulletTex), bossGiantBulletTex(giantBulletTex)
{
    switch (type)
    {
    // -------------------------------------------------------
    // 🛸 DRONE — bay ngang, ban thang xuong
    // -------------------------------------------------------
    // -------------------------------------------------------
    case EnemyType::DRONE:
        hp           = 1;
        scoreValue   = 100;
        speed        = 1.2f;
        fireInterval = 5.0f; // 5 giay ban 1 lan
        width        = tex.width;
        height       = tex.height;
        velocity     = {0, speed};  // Bay thang xuong
        image        = tex;
        hasImage     = true;
        break;

    // -------------------------------------------------------
    // 🔮 SCOUT — zic zac ngang, moi con co phase rieng
    // -------------------------------------------------------
    case EnemyType::SCOUT:
        hp           = 1;
        scoreValue   = 200;
        speed        = 1.8f;
        fireInterval = 5.0f; // 5 giay ban 1 lan
        width        = tex.width;
        height       = tex.height;
        velocity     = {0, 0};
        // Phase ngau nhien de moi con zic zac lech nhau
        timeAlive    = (float)(rand() % 628) / 100.0f;
        image        = tex;
        hasImage     = true;
        break;

    // -------------------------------------------------------
    // BOSSES — chi di trai/phai, khong roi man hinh
    // -------------------------------------------------------
    case EnemyType::MINI_BOSS:
        hp           = 25;
        scoreValue   = 1000;
        speed        = 2.0f;
        fireInterval = 1.0f;
        scale        = 2.0f; // Phong to anh drone/scout len
        tint         = WHITE; // Giu mau goc cua anh
        width        = tex.width * scale;
        height       = tex.height * scale;
        velocity     = {speed, 0};
        if (position.x > 400.0f) velocity.x = -speed; // Bay vao trong tu huong phai
        image        = tex;
        hasImage     = true;
        break;

    case EnemyType::BOSS:
        hp           = 60;
        scoreValue   = 3000;
        speed        = 2.5f;
        fireInterval = 0.8f;
        scale        = 1.0f; // Anh boss goc kha to nen de ty le 1:1
        tint         = WHITE; // Giu mau goc cua anh
        width        = tex.width * scale;
        height       = tex.height * scale;
        velocity     = {speed, 0};
        if (position.x > 400.0f) velocity.x = -speed; // Bay vao trong tu huong phai
        image        = tex;
        hasImage     = true;
        break;

    case EnemyType::GRAND_BOSS:
        hp           = 800; // Mau rat trau
        scoreValue   = 5000;
        speed        = 1.5f;
        fireInterval = 1.0f;
        scale        = 1.2f;
        tint         = WHITE;
        width        = tex.width * scale;
        height       = tex.height * scale;
        velocity     = {speed, 0}; // Di sang phai truoc
        image        = tex;
        hasImage     = true;
        break;

    case EnemyType::FINAL_BOSS:
        hp           = 200;
        scoreValue   = 10000;
        speed        = 0.0f; // Khong di chuyen
        fireInterval = 0.5f;
        scale        = 0.5f; 
        tint         = WHITE;
        width        = tex.width * scale;
        height       = tex.height * scale;
        velocity     = {0, 0}; 
        image        = tex;
        hasImage     = true;
        // Canh giua man hinh, sat mep tren
        this->position.x = (float)GetScreenWidth() / 2 - width / 2.0f;
        this->position.y = 20.0f;
        break;

    default:
        break;
    }
}

Enemy::~Enemy() {}

// -------------------------------------------------------
// UPDATE
// -------------------------------------------------------
void Enemy::Update(Vector2 playerPos)
{
    if (!alive) return;
    timeAlive += GetFrameTime();

    switch (type)
    {
    // 🛸 DRONE — bay thang xuong tu tu
    case EnemyType::DRONE:
    {
        position.y += velocity.y;   // Chi di chuyen truc Y

        if (position.y > GetScreenHeight()) {
            alive = false;
            escapedBottom = true;
        }

        FireLaser(playerPos);
        break;
    }

    // 🔮 SCOUT — zic zac ngang, tut dan xuong, moi con lech phase
    case EnemyType::SCOUT:
    {
        // Tut xuong cham
        position.y += 0.8f;

        // Zic zac ngang theo sin, phase rieng nen khong dồn vao nhau
        position.x += sinf(timeAlive * 2.5f) * 3.0f;

        // Giu trong man hinh
        if (position.x < 0) position.x = 0;
        if (position.x + width > GetScreenWidth())
            position.x = (float)(GetScreenWidth() - width);

        if (position.y > GetScreenHeight()) {
            alive = false;
            escapedBottom = true;
        }

        FireLaser(playerPos);
        break;
    }

    // CÁC LOẠI BOSS
    case EnemyType::MINI_BOSS:
    case EnemyType::GRAND_BOSS:
    {
        if (position.y < 40) {
            position.y += speed * 0.5f;
        } else {
            position.x += velocity.x;
            if (position.x <= 0) {
                position.x = 0;
                velocity.x = speed;
            }
            if (position.x + width >= GetScreenWidth()) {
                position.x = (float)(GetScreenWidth() - width);
                velocity.x = -speed;
            }
            if (position.y > 120) position.y = 120;
        }
        FireLaser(playerPos);
        break;
    }

    case EnemyType::BOSS:
    {
        float targetY = (bossIndex % 2 == 0) ? 40.0f : 100.0f;
        if (position.y < targetY) {
            position.y += speed * 0.5f;
        } else {
            // State Machine cho Boss Trung
            attackTimer += GetFrameTime();

            if (attackState == 0) {
                // State 0: Ban lien tuc hinh xoan oc
                if (attackTimer > 5.0f) {
                    attackState = 1;
                    attackTimer = 0.0f;
                }
            } else if (attackState == 1) {
                // State 1: Ban vong tron dan delay
                if (attackTimer > 4.0f) {
                    attackState = 2;
                    attackTimer = 0.0f;
                    warningTimer = 2.0f;
                }
            } else if (attackState == 2) {
                // State 2: Ban dan khong lo co canh bao
                if (warningTimer > 0.0f) {
                    warningTimer -= GetFrameTime();
                }
                if (attackTimer > 3.0f) {
                    attackState = 0;
                    attackTimer = 0.0f;
                }
            }

            position.x += velocity.x;
            if (position.x <= 0) {
                position.x = 0;
                velocity.x = speed;
            }
            if (position.x + width >= GetScreenWidth()) {
                position.x = (float)(GetScreenWidth() - width);
                velocity.x = -speed;
            }
            if (position.y > 120) position.y = 120;
        }
        FireLaser(playerPos);
        break;
    }

    case EnemyType::FINAL_BOSS:
    {
        position.x = (float)GetScreenWidth() / 2 - width / 2.0f;
        break;
    }

    default:
        break;
    }

}

// -------------------------------------------------------
// FIRE LASER
// -------------------------------------------------------
void Enemy::FireLaser(Vector2 playerPos)
{
    // Bo qua check fireInterval neu la cac boss co state machine rieng
    if (type == EnemyType::DRONE || type == EnemyType::SCOUT || type == EnemyType::GRAND_BOSS) {
        if (GetTime() - lastFireTime < fireInterval) return;
    }

    switch (type)
    {
    case EnemyType::DRONE:
        lasers.push_back(Laser(
            {position.x + width / 2.0f, position.y + height},
            {0.0f, 4.0f},
            WHITE, Laser::texDrone
        ));
        break;

    case EnemyType::SCOUT:
    {
        // Ban thang xuong
        lasers.push_back(Laser(
            {position.x + width / 2.0f, position.y + height},
            {0.0f, 6.0f},
            WHITE, Laser::texScout
        ));
        break;
    }

    case EnemyType::MINI_BOSS:
    {
        attackTimer += GetFrameTime();
        
        if (attackState == 0) {
            // Pattern 1: Ban 5 tia moi 0.8s, keo dai 2 giay
            if (GetTime() - lastFireTime > 0.8f) {
                float angles[5] = {-30.0f, -15.0f, 0.0f, 15.0f, 30.0f};
                for (int i = 0; i < 5; i++) {
                    float rad = (angles[i] + 90.0f) * PI / 180.0f;
                    Vector2 vel = { cosf(rad) * 5.0f, sinf(rad) * 5.0f };
                    lasers.push_back(Laser(
                        {position.x + width / 2.0f, position.y + height},
                        vel, WHITE, Laser::texScout
                    ));
                }
                lastFireTime = GetTime();
            }
            if (attackTimer >= 2.0f) {
                attackTimer = 0.0f;
                attackState = 1;
            }
        } else if (attackState == 1) {
            // Nghi 0.5s
            if (attackTimer >= 0.5f) {
                attackTimer = 0.0f;
                attackState = 2;
            }
        } else if (attackState == 2) {
            // Pattern 2: Ban 3 tia nhanh moi 0.4s, keo dai 2 giay
            if (GetTime() - lastFireTime > 0.4f) {
                float angles[3] = {-20.0f, 0.0f, 20.0f};
                for (int i = 0; i < 3; i++) {
                    float rad = (angles[i] + 90.0f) * PI / 180.0f; 
                    Vector2 vel = { cosf(rad) * 6.5f, sinf(rad) * 6.5f };
                    lasers.push_back(Laser(
                        {position.x + width / 2.0f, position.y + height},
                        vel, WHITE, Laser::texScout
                    ));
                }
                lastFireTime = GetTime();
            }
            if (attackTimer >= 2.0f) {
                attackTimer = 0.0f;
                attackState = 3;
            }
        } else if (attackState == 3) {
            // Nghi 0.5s
            if (attackTimer >= 0.5f) {
                attackTimer = 0.0f;
                attackState = 4;
            }
        } else if (attackState == 4) {
            // Pattern 3: Ban 3 hinh tron dong tam
            if (GetTime() - lastFireTime > 1.0f) {
                int numBullets = 12;
                float speeds[3] = { 4.0f, 5.5f, 7.0f };
                for (int ring = 0; ring < 3; ring++) {
                    for (int i = 0; i < numBullets; i++) {
                        float rad = (i * (360.0f / numBullets)) * PI / 180.0f;
                        Vector2 vel = { cosf(rad) * speeds[ring], sinf(rad) * speeds[ring] };
                        lasers.push_back(Laser(
                            {position.x + width / 2.0f, position.y + height / 2.0f},
                            vel, WHITE, Laser::texScout
                        ));
                    }
                }
                lastFireTime = GetTime();
            }
            if (attackTimer >= 2.0f) {
                attackTimer = 0.0f;
                attackState = 5;
            }
        } else if (attackState == 5) {
            // Nghi 0.5s
            if (attackTimer >= 0.5f) {
                attackTimer = 0.0f;
                attackState = 0;
            }
        }
        break;
    }

    case EnemyType::GRAND_BOSS:
    {
        // Ban vong tron 8 tia lien tuc
        int numBullets = 8;
        for (int i = 0; i < numBullets; i++) {
            float rad = (i * (360.0f / numBullets) + currentAngle) * PI / 180.0f;
            Vector2 vel = { cosf(rad) * 6.0f, sinf(rad) * 6.0f };
            lasers.push_back(Laser(
                {position.x + width / 2.0f, position.y + height / 2.0f},
                vel, Color{255, 50, 50, 255}
            ));
        }
        currentAngle += 15.0f;
        if (currentAngle >= 360.0f) currentAngle -= 360.0f;
        break;
    }

    case EnemyType::BOSS:
    {
        if (attackState == 0) {
            // Pattern 1: Ban lien tuc hinh xoan oc (interval sieu nhanh, vuot qua lastFireTime check)
            if (GetTime() - lastFireTime > 0.15f) {
                float rad = currentAngle * PI / 180.0f;
                Vector2 vel = { cosf(rad) * 6.0f, sinf(rad) * 6.0f };
                lasers.push_back(Laser(
                    {position.x + width / 2.0f, position.y + height / 2.0f},
                    vel, WHITE, bossBulletTex, 0.0f
                ));
                currentAngle += 20.0f;
                if (currentAngle >= 360.0f) currentAngle -= 360.0f;
                lastFireTime = GetTime();
            }
        } else if (attackState == 1) {
            // Pattern 2: Vong tron delay (chi ban 1 lan moi chu ky)
            if (attackTimer < 0.2f && GetTime() - lastFireTime > 1.0f) {
                int numBullets = 12;
                for (int i = 0; i < numBullets; i++) {
                    float rad = (i * (360.0f / numBullets)) * PI / 180.0f;
                    Vector2 vel = { cosf(rad) * 6.0f, sinf(rad) * 6.0f };
                    // Set delay 1.0s
                    lasers.push_back(Laser(
                        {position.x + width / 2.0f, position.y + height / 2.0f},
                        vel, WHITE, bossBulletTex, 1.0f
                    ));
                }
                lastFireTime = GetTime();
            }
        } else if (attackState == 2) {
            // Pattern 3: Dan khong lo (ban ra 1 vien sau khi het canh bao)
            if (warningTimer <= 0.0f && attackTimer < 2.5f && GetTime() - lastFireTime > 2.0f) {
                // Sinh ra tu ngay giua than Boss thay vi tren cung
                lasers.push_back(Laser(
                    {position.x + width / 2.0f, position.y + height / 2.0f},
                    Vector2{0.0f, 9.0f}, WHITE, bossGiantBulletTex, 0.0f
                ));
                lastFireTime = GetTime();
            }
        }
        return; // Thoat luon khong chay lastFireTime phia duoi
    }

    default:
        break;
    }

    lastFireTime = GetTime();
}

// -------------------------------------------------------
// DRAW WARNING ZONE
// -------------------------------------------------------
void Enemy::DrawWarningZone()
{
    // Da xoa hieu ung ve canh bao (thanh mau do) theo yeu cau.
}

// -------------------------------------------------------
// DRAW
// -------------------------------------------------------
void Enemy::Draw()
{
    if (!alive) return;

    if (hasImage)
        DrawTextureEx(image, position, 0.0f, scale, tint);
    else
        DrawRectangle((int)position.x, (int)position.y, width, height, RED);
}

// -------------------------------------------------------
// GET RECT
// -------------------------------------------------------
Rectangle Enemy::GetRect() const
{
    return { position.x, position.y, (float)width, (float)height };
}
