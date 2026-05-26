#include "laser.hpp"

Texture2D Laser::tex1 = { 0 };
Texture2D Laser::tex11 = { 0 };
Texture2D Laser::texDrone = { 0 };
Texture2D Laser::texScout = { 0 };

void Laser::LoadTextures() {
    tex1 = LoadTexture("Graphics/01.png");
    tex11 = LoadTexture("Graphics/11.png");
    texDrone = LoadTexture("Graphics/id.png");
    texScout = LoadTexture("Graphics/id.png");
}

void Laser::UnloadTextures() {
    UnloadTexture(tex1);
    UnloadTexture(tex11);
    UnloadTexture(texDrone);
    UnloadTexture(texScout);
}

Laser::Laser(Vector2 position, float speed, Color color)
{
    this->position = position;
    this->velocity = {0.0f, speed};
    this->color    = color;
    active         = true;
    startY         = position.y;
    this->customTex = {0};
    this->delayTime = 0.0f;
}

Laser::Laser(Vector2 position, Vector2 velocity, Color color, Texture2D customTex, float delayTime)
{
    this->position = position;
    this->velocity = velocity;
    this->color    = color;
    active         = true;
    startY         = position.y;
    this->customTex = customTex;
    this->delayTime = delayTime;
}

void Laser::Draw() {
    if (active) {
        if (customTex.id != 0) {
            float scale = 1.0f; // Tăng kích thước đạn lên để dễ nhìn
            DrawTextureEx(customTex, {(float)position.x - customTex.width * scale / 2.0f, (float)position.y - customTex.height * scale / 2.0f}, 0.0f, scale, WHITE);
        } else if (velocity.y < 0) { // Dan cua nguoi choi (bay len)
            float dist = startY - position.y;
            Texture2D* currentTex = (dist < 150.0f) ? &tex1 : &tex11;
            if (currentTex->id != 0) {
                DrawTextureEx(*currentTex, {(float)(position.x - (currentTex->width * 0.5f) / 2.0f), (float)position.y}, 0.0f, 0.5f, WHITE);
            } else {
                DrawRectangle((int)position.x, (int)position.y, 4, 8, color);
            }
        } else {
            // Dan cua dich (bay xuong)
            DrawRectangle((int)position.x, (int)position.y, 4, 15, color);
        }
    }
}

void Laser::Update() {
    if (delayTime > 0.0f) {
        delayTime -= GetFrameTime();
    } else {
        position.x += velocity.x;
        position.y += velocity.y;
    }

    if (active) {
        if (position.y > GetScreenHeight() + 200 || position.y < -200 ||
            position.x > GetScreenWidth() + 200 || position.x < -200)
            active = false;
    }
}

Rectangle Laser::GetRect() const
{
    if (customTex.id != 0) {
        return { position.x - customTex.width / 2.0f, position.y - customTex.height / 2.0f, (float)customTex.width, (float)customTex.height };
    }
    return { position.x, position.y, 4, 15 };
}
