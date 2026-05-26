#include "asteroid.hpp"
#include "game.hpp"
#include <cstdlib>
#include <algorithm>

void AsteroidManager::Spawn(Game& g) {
    if (g.state != GameState::PLAYING) return;
    g.asteroidSpawnTimer -= GetFrameTime();
    if (g.asteroidSpawnTimer <= 0) {
        Asteroid a;
        a.radius        = (float)(rand() % 20 + 15);
        a.position      = {(float)(rand() % GetScreenWidth()), -a.radius - 10};
        a.speed         = (g.currentLevel * 60.0f) + (g.currentPhase * 30.0f) + (rand() % 40);
        a.rotation      = (float)(rand() % 360);
        a.rotationSpeed = (float)(rand() % 150 - 75);
        a.active        = true;
        g.asteroids.push_back(a);
        g.asteroidSpawnTimer = g.asteroidSpawnInterval + (float)(rand() % 100) / 100.0f;
    }
}

void AsteroidManager::Update(Game& g) {
    for (auto& a : g.asteroids) {
        if (!a.active) continue;
        a.position.y += a.speed * GetFrameTime();
        a.rotation   += a.rotationSpeed * GetFrameTime();
        if (a.position.y > GetScreenHeight() + a.radius) a.active = false;
    }
    g.asteroids.erase(
        std::remove_if(g.asteroids.begin(), g.asteroids.end(), [](const Asteroid& a){ return !a.active; }),
        g.asteroids.end()
    );
}

void AsteroidManager::Draw(Game& g) {
    for (const auto& a : g.asteroids) {
        if (!a.active) continue;
        Rectangle src  = {0,0,(float)g.texAsteroid.width,(float)g.texAsteroid.height};
        Rectangle dst  = {a.position.x, a.position.y, a.radius*2.5f, a.radius*2.5f};
        Vector2   orig = {a.radius*1.25f, a.radius*1.25f};
        DrawTexturePro(g.texAsteroid, src, dst, orig, a.rotation, WHITE);
    }
}
