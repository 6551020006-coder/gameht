#pragma once
#include <raylib.h>

class Laser {
public:
    Laser(Vector2 position, float speed, Color color = {243, 216, 63, 255});
    Laser(Vector2 position, Vector2 velocity, Color color = {243, 216, 63, 255}, Texture2D customTex = {0}, float delayTime = 0.0f);
    void Draw();
    void Update();
    Rectangle GetRect() const;  // Them de kiem tra collision
    bool active;
    Texture2D customTex;
    float delayTime;
    static Texture2D tex1;
    static Texture2D tex11;
    static Texture2D texDrone;
    static Texture2D texScout;
    static void LoadTextures();
    static void UnloadTextures();

private:
    Vector2 position;
    Vector2 velocity;
    Color color;
    float startY;
};
