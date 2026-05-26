#include "particle_manager.hpp"
#include "game.hpp"
#include <cstdlib>
#include <cmath>
#include <algorithm>

void ParticleManager::SpawnExplosion(Game& g, Vector2 pos, Color color) {
    StopSound(g.sfxExplode);
    PlaySound(g.sfxExplode);
    int count = 15 + rand() % 10;
    for (int i = 0; i < count; i++) {
        Particle p;
        p.position = pos;
        float angle = (float)(rand() % 360) * DEG2RAD;
        float speed = (float)(rand() % 150 + 50);
        p.velocity = {cosf(angle) * speed, sinf(angle) * speed};
        p.color = color;
        p.life  = 1.0f;
        p.size  = (float)(rand() % 4 + 2);
        g.particles.push_back(p);
    }
}

void ParticleManager::Update(Game& g) {
    float dt = GetFrameTime();
    for (auto& p : g.particles) {
        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;
        p.life -= dt * 1.5f;
    }
    g.particles.erase(
        std::remove_if(g.particles.begin(), g.particles.end(), [](const Particle& p){ return p.life <= 0; }),
        g.particles.end()
    );
}

void ParticleManager::Draw(Game& g) {
    BeginBlendMode(BLEND_ADDITIVE);
    for (const auto& p : g.particles) {
        Color c = p.color;
        c.a = (unsigned char)(255 * p.life);
        DrawCircleV(p.position, p.size, c);
    }
    EndBlendMode();
}
