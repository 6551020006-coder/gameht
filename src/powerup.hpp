#pragma once
#include <raylib.h>

enum class PowerupType {
    HEAL,        // Hoi +1 HP
    RAPID_FIRE,  // Ban 3 vien/lan trong 6 giay
    SHIELD,      // Chan dan trong 5 giay
    BOMB         // Xoa toan bo enemy tren man hinh
};

class Powerup {
public:
    Powerup(Vector2 position, PowerupType type, Texture2D tex);
    void Update();
    void Draw();
    Rectangle GetRect() const;

    // Thu tu khai bao phai khop voi initializer list trong .cpp:
    // position, type, active, speed, size, image, pulse
    PowerupType type;
    bool active;

private:
    Vector2 position;
    float speed;
    int size;
    Texture2D image;
    float pulse;
};
