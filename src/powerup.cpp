#include "powerup.hpp"
#include <cmath>

Powerup::Powerup(Vector2 position, PowerupType type, Texture2D tex)
    : type(type), active(true),
      position(position), speed(1.5f), size(48), image(tex), pulse(0.0f)
{}

void Powerup::Update() {
    if (!active) return;
    position.y += 2.0f; // Toc do roi

    // Tu huy neu roi khoi man hinh
    if (position.y > GetScreenHeight() + 50) {
        active = false;
    }
    pulse += GetFrameTime() * 4.0f; // Nhip tim nhanh
}

void Powerup::Draw() {
    if (!active) return;

    // Hieu ung nhip tim: scale dao dong nhe
    float scale = 1.0f + sinf(pulse) * 0.06f;
    int drawSize = (int)(size * scale);
    int offset   = (drawSize - size) / 2;

    // Ve nen phat sang theo loai skill
    Color glowColor = {255, 255, 255, 60}; // default
    switch (type) {
    case PowerupType::SHIELD:     glowColor = Color{60,  140, 255, 60}; break;
    case PowerupType::RAPID_FIRE: glowColor = Color{255, 200,   0, 60}; break;
    case PowerupType::HEAL:       glowColor = Color{ 50, 220,  80, 60}; break;
    case PowerupType::BOMB:       glowColor = Color{255,  80,  30, 60}; break;
    }

    // Ve vong sang phia sau
    DrawCircle(
        (int)(position.x + size / 2),
        (int)(position.y + size / 2),
        (float)(drawSize / 2 + 6),
        glowColor
    );

    // Ve texture chinh co scale nhip tim
    Rectangle src  = {0, 0, (float)image.width, (float)image.height};
    Rectangle dst  = {
        position.x - offset,
        position.y - offset,
        (float)drawSize,
        (float)drawSize
    };
    DrawTexturePro(image, src, dst, {0, 0}, 0.0f, WHITE);
}

Rectangle Powerup::GetRect() const {
    return { position.x, position.y, (float)size, (float)size };
}
