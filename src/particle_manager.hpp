#pragma once
#include <raylib.h>

class Game;

class ParticleManager {
public:
    static void SpawnExplosion(Game& g, Vector2 pos, Color color);
    static void Update(Game& g);
    static void Draw(Game& g);
};
