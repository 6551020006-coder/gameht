#pragma once
#include <raylib.h>

class Game;

class AsteroidManager {
public:
    static void Spawn(Game& g);
    static void Update(Game& g);
    static void Draw(Game& g);
};
