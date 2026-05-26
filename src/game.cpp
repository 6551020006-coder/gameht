#include "game.hpp"
#include "ui_manager.hpp"
#include "level_manager.hpp"
#include "particle_manager.hpp"
#include "asteroid.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <stdexcept>

// -------------------------------------------------------
// Helper noi bo
// -------------------------------------------------------
void Game::DrawScrollingBgPublic(Texture2D& tex, float scrollY) {
    float sW = (float)GetScreenWidth(),  sH = (float)GetScreenHeight();
    float tW = (float)tex.width,         tH = (float)tex.height;
    float srcY = fmodf(scrollY, tH);
    if (srcY < 0) srcY += tH;
    float remaining = tH - srcY;
    if (remaining >= sH) {
        DrawTexturePro(tex, {0,srcY,tW,sH}, {0,0,sW,sH}, {0,0}, 0, WHITE);
    } else {
        float destH1 = remaining / sH * sH;
        DrawTexturePro(tex, {0,srcY,tW,remaining}, {0,0,sW,destH1}, {0,0}, 0, WHITE);
        float srcH2 = sH - remaining;
        DrawTexturePro(tex, {0,0,tW,srcH2}, {0,destH1,sW,sH-destH1}, {0,0}, 0, WHITE);
    }
}

// -------------------------------------------------------
// AM THANH
// -------------------------------------------------------
void Game::PlayBGM() {
    if (bgmPlaying) return;
    bgmPlaying = true;
    PlayMusicStream(bgm[currentBgm]);
    SetMusicVolume(bgm[currentBgm], 0.5f);
}

void Game::StopBGM() {
    if (!bgmPlaying) return;
    bgmPlaying = false;
    StopMusicStream(bgm[currentBgm]);
}

void Game::UpdateBGM() {
    if (!bgmPlaying) return;
    UpdateMusicStream(bgm[currentBgm]);
    if (GetMusicTimePlayed(bgm[currentBgm]) >= GetMusicTimeLength(bgm[currentBgm]) - 0.05f) {
        StopMusicStream(bgm[currentBgm]);
        currentBgm = (currentBgm + 1) % 3;
        PlayMusicStream(bgm[currentBgm]);
        SetMusicVolume(bgm[currentBgm], 0.5f);
    }
}

// -------------------------------------------------------
// CONSTRUCTOR
// -------------------------------------------------------
Game::Game() {
    Laser::LoadTextures();
    try {
        droneTexture   = LoadTexture("Graphics/drone.png");
        scoutTexture   = LoadTexture("Graphics/scout.png");
        auto initAnim = [](const char* path, AnimTexture& anim) {
            anim.animFrames = 0;
            anim.animImage  = LoadImageAnim(path, &anim.animFrames);
            anim.texture    = (anim.animImage.data != nullptr) ? LoadTextureFromImage(anim.animImage) : Texture2D{0};
            anim.currentFrame = 0; anim.frameTimer = 0.0f; anim.frameDuration = 0.08f;
        };

        for (int i = 0; i < 4; i++) {
            initAnim(TextFormat("Graphics/boss %d.gif", i+1), bossTextures[i]);
            initAnim(TextFormat("Graphics/dan%d.gif",   i+1), bossGiantBulletTex[i]);
            const char* gateName[] = {"gate01a","gate01b","gate01c","gate01d"};
            initAnim(TextFormat("Graphics/%s.gif", gateName[i]), bossBulletTex[i]);
        }
        initAnim("Graphics/boss 5.gif",  boss5Tex);
        initAnim("Graphics/samset.gif",  samsetTex);
        initAnim("Graphics/gate01a.gif", finalBossBullets[0]);
        initAnim("Graphics/gate01b.gif", finalBossBullets[1]);
        initAnim("Graphics/gate01c.gif", finalBossBullets[2]);
        initAnim("Graphics/gate01d.gif", finalBossBullets[3]);
        initAnim("Graphics/ic.gif",      finalBossBullets[4]);

        texAsteroid  = LoadTexture("Graphics/tt.png");
        bgTexture    = LoadTexture("Graphics/galaxy_seamless.png");
        if (droneTexture.id==0||scoutTexture.id==0||bgTexture.id==0||bossTextures[0].texture.id==0||texAsteroid.id==0)
            throw std::runtime_error("Loi: Khong the load hinh anh trong Graphics/");
        SetTextureFilter(bgTexture, TEXTURE_FILTER_BILINEAR);

        texStarQuaman = LoadTexture("Graphics/starquaman.png");
        if (texStarQuaman.id == 0) throw std::runtime_error("Loi: Khong the load Graphics/starquaman.png");

        texShield    = LoadTexture("Graphics/shield.png");
        texRapidFire = LoadTexture("Graphics/rapidfire.png");
        texHeal      = LoadTexture("Graphics/heal.png");
        texBomb      = LoadTexture("Graphics/bomb.png");
        if (texShield.id==0||texRapidFire.id==0||texHeal.id==0||texBomb.id==0)
            throw std::runtime_error("Loi: Khong the load Powerup textures");

        shipSelectTextures[0] = LoadTexture("Graphics/spaceship.png");
        shipSelectTextures[1] = LoadTexture("Graphics/spaceship 1.png");
        shipSelectTextures[2] = LoadTexture("Graphics/spaceship 2.png");
        if (shipSelectTextures[0].id==0||shipSelectTextures[1].id==0||shipSelectTextures[2].id==0)
            throw std::runtime_error("Loi: Khong the load ship select textures");

        sfxLaser     = LoadSound("Sounds/laser.wav");
        sfxExplode   = LoadSound("Sounds/explosion.wav");
        sfxEnemyLaser= LoadSound("Sounds/enemy_laser.wav");
        sfxHit       = LoadSound("Sounds/hit.wav");
        sfxPowerup   = LoadSound("Sounds/powerup.wav");
        sfxHurt      = LoadSound("Sounds/hurt.wav");
        sfxBoss      = LoadSound("Sounds/boss.wav");
        sfxWin       = LoadSound("Sounds/win.wav");
        if (sfxLaser.stream.buffer==nullptr||sfxExplode.stream.buffer==nullptr||sfxWin.stream.buffer==nullptr)
            throw std::runtime_error("Loi: Khong the load am thanh");

        SetSoundVolume(sfxLaser,1.f); SetSoundVolume(sfxExplode,1.f); SetSoundVolume(sfxEnemyLaser,1.f);
        SetSoundVolume(sfxHit,1.f);   SetSoundVolume(sfxPowerup,1.f); SetSoundVolume(sfxHurt,1.f);
        SetSoundVolume(sfxBoss,1.f);  SetSoundVolume(sfxWin,1.f);

        for (int i = 0; i < 3; i++) {
            bgm[i] = LoadMusicStream(TextFormat("Sounds/bgm%d.mp3", i+1));
            if (bgm[i].stream.buffer==nullptr) throw std::runtime_error("Loi: Khong the load bgm");
            bgm[i].looping = false;
        }
    } catch (const std::exception& e) {
        TraceLog(LOG_ERROR, "NGOAI LE (Game Init): %s", e.what());
        throw;
    }

    currentBgm = 0; bgmPlaying = false; currentVolume = 50;
    bgY = 0.0f; score = 0;
    LevelManager::LoadData(*this);

    state = GameState::MENU;
    menuBgY = 0.0f; menuPulse = 0.0f; gameOverPulse = 0.0f; victoryPulse = 0.0f;
    godMode = false; isGameActive = false;
    currentMenuOption = 0; currentPauseOption = 0; currentSettingsOption = 0; currentShipSelectOption = 0;
    stateBeforeSettings = GameState::MENU;
    cameraShakeTimer = 0.0f;
    powerupsDropped = 0; maxPowerupsPerWave = 0; lastPowerupDropTime = 0.0; nextPowerupInterval = 0.0f;
    currentLevel = 1;
    asteroidSpawnTimer = 0.0f; asteroidSpawnInterval = 2.0f;
    PlayBGM();
}

// -------------------------------------------------------
// DESTRUCTOR
// -------------------------------------------------------
Game::~Game() {
    Laser::UnloadTextures();
    UnloadTexture(droneTexture); UnloadTexture(scoutTexture);
    for (int i = 0; i < 4; i++) {
        UnloadTexture(bossTextures[i].texture);      UnloadImage(bossTextures[i].animImage);
        UnloadTexture(bossBulletTex[i].texture);     UnloadImage(bossBulletTex[i].animImage);
        UnloadTexture(bossGiantBulletTex[i].texture);UnloadImage(bossGiantBulletTex[i].animImage);
    }
    UnloadTexture(boss5Tex.texture);  UnloadImage(boss5Tex.animImage);
    UnloadTexture(samsetTex.texture); UnloadImage(samsetTex.animImage);
    for (int i=0;i<5;i++) { UnloadTexture(finalBossBullets[i].texture); UnloadImage(finalBossBullets[i].animImage); }
    UnloadTexture(texAsteroid); UnloadTexture(bgTexture); UnloadTexture(texStarQuaman);
    UnloadTexture(texShield); UnloadTexture(texRapidFire); UnloadTexture(texHeal); UnloadTexture(texBomb);
    for (int i=0;i<3;i++) UnloadTexture(shipSelectTextures[i]);
    UnloadSound(sfxLaser); UnloadSound(sfxExplode); UnloadSound(sfxEnemyLaser);
    UnloadSound(sfxHit); UnloadSound(sfxPowerup); UnloadSound(sfxHurt); UnloadSound(sfxBoss); UnloadSound(sfxWin);
    for (int i=0;i<3;i++) UnloadMusicStream(bgm[i]);
}

// -------------------------------------------------------
// UPDATE ANIMATIONS
// -------------------------------------------------------
void Game::UpdateAnimations() {
    float dt = GetFrameTime();
    auto updateAnim = [dt](AnimTexture& anim) {
        if (anim.animFrames > 1 && anim.animImage.data != nullptr) {
            anim.frameTimer += dt;
            if (anim.frameTimer >= anim.frameDuration) {
                anim.currentFrame++;
                if (anim.currentFrame >= anim.animFrames) anim.currentFrame = 0;
                int offset = anim.animImage.width * anim.animImage.height * 4 * anim.currentFrame;
                UpdateTexture(anim.texture, ((unsigned char*)anim.animImage.data) + offset);
                anim.frameTimer = 0.0f;
            }
        }
    };
    for (int i=0;i<4;i++) { updateAnim(bossTextures[i]); updateAnim(bossBulletTex[i]); updateAnim(bossGiantBulletTex[i]); }
    updateAnim(boss5Tex); updateAnim(samsetTex);
    for (int i=0;i<5;i++) updateAnim(finalBossBullets[i]);
}

// -------------------------------------------------------
// COLLISION
// -------------------------------------------------------
void Game::CheckCollisions() {
    if (state != GameState::PLAYING) return;

    // Player vs Asteroid
    if (!godMode && spaceship.invulnerableTimer <= 0) {
        Vector2 pPos = {spaceship.GetPosition().x + spaceship.GetWidth()/2.0f, spaceship.GetPosition().y + spaceship.GetHeight()/2.0f};
        float pRadius = spaceship.GetWidth() / 2.5f;
        for (auto& a : asteroids) {
            if (!a.active) continue;
            if (CheckCollisionCircles(pPos, pRadius, a.position, a.radius*0.8f)) {
                a.active = false; spaceship.hp -= 10; spaceship.invulnerableTimer = 1.0f;
                PlaySound(sfxHurt); cameraShakeTimer = 0.3f;
                ParticleManager::SpawnExplosion(*this, a.position, GRAY);
                break;
            }
        }
    }

    for (auto& laser : spaceship.lasers) {
        if (!laser.active) continue;
        for (auto& enemy : enemies) {
            if (!enemy.alive || enemy.GetRect().y < 0) continue;
            if (CheckCollisionRecs(laser.GetRect(), enemy.GetRect())) {
                laser.active = false;
                enemy.hp -= 1;
                if (enemy.hp <= 0) {
                    enemy.alive = false;
                    score += enemy.scoreValue;
                    StopSound(sfxExplode); PlaySound(sfxExplode);
                    ParticleManager::SpawnExplosion(*this, {enemy.GetRect().x + enemy.GetRect().width/2, enemy.GetRect().y + enemy.GetRect().height/2}, ORANGE);
                    if (enemy.type==EnemyType::MINI_BOSS||enemy.type==EnemyType::BOSS||enemy.type==EnemyType::GRAND_BOSS||enemy.type==EnemyType::FINAL_BOSS)
                        bossesKilledInPhase++;
                } else { StopSound(sfxHit); PlaySound(sfxHit); }
                break;
            }
        }
    }
}

void Game::CheckEnemyLaserCollisions() {
    if (spaceship.shieldActive || godMode || spaceship.invulnerableTimer > 0.0f) return;
    Rectangle playerRect = {spaceship.GetPosition().x, spaceship.GetPosition().y, (float)spaceship.GetWidth(), (float)spaceship.GetHeight()};

    for (auto& laser : enemyLasers) {
        if (!laser.active) continue;
        if (CheckCollisionRecs(laser.GetRect(), playerRect)) {
            laser.active = false; spaceship.hp -= 5; spaceship.invulnerableTimer = 1.0f;
            StopSound(sfxHurt); PlaySound(sfxHurt); cameraShakeTimer = 0.3f; return;
        }
    }
    for (auto& enemy : enemies) {
        if (!enemy.alive) continue;
        if (CheckCollisionRecs(enemy.GetRect(), playerRect)) {
            if (enemy.type!=EnemyType::MINI_BOSS && enemy.type!=EnemyType::BOSS && enemy.type!=EnemyType::GRAND_BOSS) {
                enemy.alive = false; StopSound(sfxExplode); PlaySound(sfxExplode);
                ParticleManager::SpawnExplosion(*this, {enemy.GetRect().x + enemy.GetRect().width/2, enemy.GetRect().y + enemy.GetRect().height/2}, ORANGE);
            }
            spaceship.hp -= 10; spaceship.invulnerableTimer = 1.0f;
            StopSound(sfxHurt); PlaySound(sfxHurt); cameraShakeTimer = 0.3f; return;
        }
    }
}

void Game::CheckPowerupCollisions() {
    Rectangle playerRect = {spaceship.GetPosition().x, spaceship.GetPosition().y, (float)spaceship.GetWidth(), (float)spaceship.GetHeight()};
    for (auto& p : powerups) {
        if (!p.active || !CheckCollisionRecs(playerRect, p.GetRect())) continue;
        p.active = false; StopSound(sfxPowerup); PlaySound(sfxPowerup);
        spaceship.inventory[(int)p.type]++;
    }
}

// -------------------------------------------------------
// USE BOMB
// -------------------------------------------------------
void Game::UseBomb() {
    PlaySound(sfxExplode);
    for (auto& e : enemies) {
        if (!e.alive || e.GetRect().y <= 0) continue;
        if (e.type!=EnemyType::MINI_BOSS && e.type!=EnemyType::BOSS && e.type!=EnemyType::GRAND_BOSS && e.type!=EnemyType::FINAL_BOSS) {
            e.alive = false; score += e.scoreValue;
            ParticleManager::SpawnExplosion(*this, {e.GetRect().x+e.GetRect().width/2, e.GetRect().y+e.GetRect().height/2}, ORANGE);
        } else {
            e.hp -= 20;
            if (e.hp <= 0) {
                e.alive = false; bossesKilledInPhase++; score += e.scoreValue;
                ParticleManager::SpawnExplosion(*this, {e.GetRect().x+e.GetRect().width/2, e.GetRect().y+e.GetRect().height/2}, ORANGE);
            } else {
                ParticleManager::SpawnExplosion(*this, {e.GetRect().x+e.GetRect().width/2, e.GetRect().y+e.GetRect().height/2}, PURPLE);
            }
        }
    }
    StopSound(sfxExplode); PlaySound(sfxExplode); cameraShakeTimer = 0.5f;
}

// -------------------------------------------------------
// DELETE INACTIVE LASERS
// -------------------------------------------------------
void Game::DeleteInactiveLasers() {
    spaceship.lasers.erase(
        std::remove_if(spaceship.lasers.begin(), spaceship.lasers.end(), [](const Laser& l){ return !l.active; }),
        spaceship.lasers.end());
}

// -------------------------------------------------------
// UPDATE FINAL BOSS
// -------------------------------------------------------
void Game::UpdateFinalBoss(Enemy& boss) {
    float dt = GetFrameTime();
    boss.attackTimer += dt;
    Vector2 pPos = spaceship.GetPosition();
    Vector2 bPos = {boss.GetRect().x + boss.GetRect().width/2, boss.GetRect().y + boss.GetRect().height/2};

    for (auto& beam : beams) {
        if (!beam.isActive) continue;
        beam.currentLife -= dt;
        if (beam.currentLife <= 0) beam.isActive = false;
        else if (beam.currentLife < beam.maxLife * 0.8f) beam.isDamaging = true;
    }
    beams.erase(std::remove_if(beams.begin(), beams.end(), [](const LightningBeam& b){ return !b.isActive; }), beams.end());

    switch (boss.attackState) {
    case 0:
        if (boss.attackTimer > 2.0f) { boss.attackState=1; boss.attackTimer=0; } break;
    case 1:
        if (boss.attackTimer < 1.5f) {
            if (rand()%3==0) {
                Particle p; float angle=(rand()%360)*DEG2RAD;
                p.position={bPos.x+cosf(angle)*100, bPos.y+sinf(angle)*100};
                p.velocity={-cosf(angle)*150,-sinf(angle)*150}; p.color=PURPLE; p.life=0.6f; p.size=3.0f;
                particles.push_back(p);
            }
        } else if (boss.attackTimer > 1.5f && boss.attackTimer < 3.5f) {
            if (boss.attackTimer>=1.6f && boss.attackTimer-dt<1.6f) {
                for(int i=0;i<36;i++) {
                    float rad=(i*10.0f)*DEG2RAD;
                    boss.lasers.push_back(Laser(bPos,{cosf(rad)*4.0f,sinf(rad)*4.0f},WHITE,finalBossBullets[4].texture));
                }
                PlaySound(sfxBoss);
            }
            if (fmodf(boss.attackTimer,0.1f)<dt) {
                float rad=boss.attackTimer*10.0f;
                boss.lasers.push_back(Laser(bPos,{cosf(rad)*5.0f,sinf(rad)*5.0f},WHITE,finalBossBullets[1].texture));
            }
        } else if (boss.attackTimer>=3.5f) { boss.attackState=2; boss.attackTimer=0; }
        break;
    case 2: if (boss.attackTimer>1.5f){boss.attackState=3;boss.attackTimer=0;} break;
    case 3: {
        int wave=(int)(boss.attackTimer/1.5f);
        if (wave<3) {
            float localTime=fmodf(boss.attackTimer,1.5f);
            if (localTime<dt) {
                LightningBeam w; w.position={pPos.x+spaceship.GetWidth()/2.0f,0}; w.width=80.0f;
                w.maxLife=0.8f; w.currentLife=0.8f; w.isActive=true; w.isDamaging=true;
                w.type=0; w.angle=0; w.length=(float)GetScreenHeight(); w.color=WHITE;
                beams.push_back(w); PlaySound(sfxExplode);
            }
        } else if (boss.attackTimer>4.5f){boss.attackState=4;boss.attackTimer=0;}
        break;
    }
    case 4: if (boss.attackTimer>1.5f){boss.attackState=5;boss.attackTimer=0;} break;
    case 5: {
        int wave=(int)(boss.attackTimer/2.0f);
        if (wave<3) {
            float localTime=fmodf(boss.attackTimer,2.0f);
            if (localTime<dt) {
                float offsetAngle=(wave%2==0)?0.0f:36.0f;
                for (int i=0;i<5;i++) {
                    float rad=(i*72.0f-90.0f+offsetAngle)*DEG2RAD; float radius=120.0f;
                    Vector2 pos={bPos.x+cosf(rad)*radius, bPos.y+sinf(rad)*radius};
                    boss.lasers.push_back(Laser(pos,{cosf(rad)*6.5f,sinf(rad)*6.5f},WHITE,finalBossBullets[1].texture,1.0f));
                }
                PlaySound(sfxEnemyLaser);
            }
        } else if (boss.attackTimer>6.0f){boss.attackState=6;boss.attackTimer=0;}
        break;
    }
    case 6: if (boss.attackTimer>1.5f){boss.attackState=7;boss.attackTimer=0;} break;
    case 7: {
        if (boss.attackTimer<4.0f) {
            if (fmodf(boss.attackTimer,0.15f)<dt) {
                float rx=(float)(rand()%GetScreenWidth());
                int tIdx=rand()%5;
                boss.lasers.push_back(Laser({rx,0},{0,3.0f+(rand()%4)},WHITE,finalBossBullets[tIdx].texture));
            }
        } else if (boss.attackTimer>5.0f){boss.attackState=0;boss.attackTimer=0;}
        break;
    }
    }

    if (!godMode) {
        Rectangle pRect={pPos.x,pPos.y,(float)spaceship.GetWidth(),(float)spaceship.GetHeight()};
        Vector2 pCenter={pPos.x+spaceship.GetWidth()/2.0f, pPos.y+spaceship.GetHeight()/2.0f};
        for (auto& b : beams) {
            if (!b.isActive || !b.isDamaging) continue;
            bool hit = false;
            if (b.type==0) {
                Rectangle bRect={b.position.x-b.width/2, b.position.y, b.width, (float)GetScreenHeight()};
                if (CheckCollisionRecs(pRect,bRect)) hit=true;
            }
            if (hit && spaceship.invulnerableTimer<=0) {
                spaceship.hp-=20; spaceship.invulnerableTimer=1.0f; cameraShakeTimer=0.5f; PlaySound(sfxHurt);
            }
        }
    }
}

// -------------------------------------------------------
// UPDATE (VÒNG LẶP CHÍNH)
// -------------------------------------------------------
void Game::Update() {
    if (bgmPlaying) UpdateMusicStream(bgm[currentBgm]);
    UpdateBGM();

    float vol = currentVolume / 100.0f;
    SetMusicVolume(bgm[currentBgm], vol*0.5f);
    SetSoundVolume(sfxLaser,vol); SetSoundVolume(sfxExplode,vol); SetSoundVolume(sfxEnemyLaser,vol);
    SetSoundVolume(sfxHit,vol);   SetSoundVolume(sfxPowerup,vol); SetSoundVolume(sfxHurt,vol);
    SetSoundVolume(sfxBoss,vol);  SetSoundVolume(sfxWin,vol);

    HandleInput();
    UpdateAnimations();

    bool isAltHeld = IsKeyDown(KEY_LEFT_ALT)||IsKeyDown(KEY_RIGHT_ALT);
    (state==GameState::PLAYING && !isAltHeld) ? HideCursor() : ShowCursor();

    if (state==GameState::MENU||state==GameState::INSTRUCTIONS||state==GameState::SETTINGS) {
        menuBgY -= 120.0f*GetFrameTime(); if (menuBgY<0) menuBgY+=bgTexture.height;
        menuPulse += GetFrameTime()*3.0f; return;
    }
    if (state==GameState::LEVEL_SELECT||state==GameState::SHIP_SELECT) {
        menuBgY -= 100.0f*GetFrameTime(); if (menuBgY<0) menuBgY+=bgTexture.height; return;
    }
    if (state==GameState::GAME_OVER) {
        menuBgY -= 80.0f*GetFrameTime(); if (menuBgY<0) menuBgY+=bgTexture.height;
        gameOverPulse += GetFrameTime()*3.0f; return;
    }
    if (state==GameState::VICTORY) {
        menuBgY -= 60.0f*GetFrameTime(); if (menuBgY<0) menuBgY+=bgTexture.height;
        victoryPulse += GetFrameTime()*2.5f; return;
    }
    if (state==GameState::PAUSED) return;

    Vector2 playerPos = spaceship.GetPosition();
    spaceship.Update();
    for (auto& laser : spaceship.lasers) laser.Update();

    LevelManager::UpdateSpawner(*this);
    LevelManager::UpdatePowerupSpawner(*this);
    AsteroidManager::Spawn(*this);
    AsteroidManager::Update(*this);

    for (auto& enemy : enemies) {
        if (enemy.type==EnemyType::FINAL_BOSS) UpdateFinalBoss(enemy);
        enemy.Update(playerPos);
        for (const auto& laser : enemy.lasers) enemyLasers.push_back(laser);
        enemy.lasers.clear();
    }

    for (auto& laser : enemyLasers) laser.Update();
    enemyLasers.erase(std::remove_if(enemyLasers.begin(),enemyLasers.end(),[](const Laser& l){return !l.active;}),enemyLasers.end());

    for (auto& p : powerups) p.Update();
    powerups.erase(std::remove_if(powerups.begin(),powerups.end(),[](const Powerup& p){return !p.active;}),powerups.end());

    ParticleManager::Update(*this);

    if (cameraShakeTimer>0.0f) { cameraShakeTimer-=GetFrameTime(); if(cameraShakeTimer<0)cameraShakeTimer=0; }

    CheckCollisions(); CheckEnemyLaserCollisions(); CheckPowerupCollisions();

    for (auto& enemy : enemies) {
        if (!enemy.escapedBottom) continue;
        if (!godMode) { spaceship.hp-=5; cameraShakeTimer=0.3f; StopSound(sfxHurt); PlaySound(sfxHurt); }
    }
    enemies.erase(std::remove_if(enemies.begin(),enemies.end(),[](const Enemy& e){return !e.alive;}),enemies.end());

    LevelManager::UpdateWaveLogic(*this);

    bgY -= 200.0f*GetFrameTime(); if (bgY<0) bgY+=bgTexture.height;

    if (spaceship.hp <= 0) {
        gameOverPulse=0.0f; menuBgY=bgY; state=GameState::GAME_OVER;
        if (score>levels[currentLevel-1].highScore) levels[currentLevel-1].highScore=score;
        if (score>highScore) highScore=score;
        LevelManager::SaveData(*this); return;
    }
    DeleteInactiveLasers();
}

// -------------------------------------------------------
// DRAW (VÒNG LẶP CHÍNH)
// -------------------------------------------------------
void Game::Draw() {
    switch (state) {
    case GameState::MENU:         UIManager::DrawMenu(*this);         return;
    case GameState::LEVEL_SELECT: UIManager::DrawLevelSelect(*this);  return;
    case GameState::SHIP_SELECT:  UIManager::DrawShipSelect(*this);   return;
    case GameState::INSTRUCTIONS: UIManager::DrawInstructions(*this); return;
    case GameState::GAME_OVER:    UIManager::DrawGameOver(*this);     return;
    case GameState::VICTORY:      UIManager::DrawVictory(*this);      return;
    case GameState::SETTINGS:     UIManager::DrawSettings(*this);     return;
    default: break;
    }

    Camera2D camera = {0}; camera.zoom = 1.0f;
    if (cameraShakeTimer > 0.0f) {
        camera.offset.x = (float)(rand()%11-5);
        camera.offset.y = (float)(rand()%11-5);
    }
    BeginMode2D(camera);
    DrawScrollingBgPublic(bgTexture, bgY);

    for (auto& p : powerups) p.Draw();
    spaceship.Draw();
    AsteroidManager::Draw(*this);

    for (auto& laser : spaceship.lasers) laser.Draw();
    for (auto& enemy : enemies) {
        UIManager::DrawHealthBar(*this, enemy);
        enemy.DrawWarningZone();
        enemy.Draw();
    }
    for (auto& laser : enemyLasers) laser.Draw();

    // Beams (Final Boss)
    for (const auto& beam : beams) {
        if (!beam.isActive) continue;
        float alpha = beam.currentLife / beam.maxLife;
        if (alpha>1.0f) alpha=1.0f;
        if (beam.type==0) {
            float drawWidth=250.0f;
            Rectangle src={0,0,(float)samsetTex.texture.width,(float)samsetTex.texture.height};
            Rectangle dst={beam.position.x-drawWidth/2.0f, beam.position.y, drawWidth, (float)GetScreenHeight()};
            DrawTexturePro(samsetTex.texture, src, dst, {0,0}, 0.0f, Fade(WHITE,alpha));
        }
    }

    ParticleManager::Draw(*this);
    EndMode2D();

    UIManager::DrawHUD(*this);
    if (state==GameState::PAUSED) UIManager::DrawPaused(*this);
}

// -------------------------------------------------------
// HANDLE INPUT
// -------------------------------------------------------
void Game::HandleInput() {
    bool shiftDown = IsKeyDown(KEY_LEFT_SHIFT)||IsKeyDown(KEY_RIGHT_SHIFT);
    if (shiftDown && IsKeyPressed(KEY_G)) {
        godMode = !godMode;
        if (godMode) { if (spaceship.hp<50) spaceship.hp=50; StopSound(sfxPowerup); PlaySound(sfxPowerup); }
    }
    if (IsKeyPressed(KEY_M)) {
        static bool muted=false; muted=!muted;
        SetMusicVolume(bgm[currentBgm], muted?0.0f:0.5f);
    }

    // ---- MENU ----
    if (state==GameState::MENU) {
        if (IsKeyPressed(KEY_UP)) { currentMenuOption--; if(currentMenuOption<0)currentMenuOption=4; }
        if (IsKeyPressed(KEY_DOWN)) { currentMenuOption++; if(currentMenuOption>4)currentMenuOption=0; }
        if (IsKeyPressed(KEY_ENTER)) {
            if (currentMenuOption==0) {
                int hi=1; for(int i=0;i<3;i++) if(levels[i].isUnlocked) hi=i+1;
                isGameActive=true; state=GameState::PLAYING; score=0; godMode=false;
                spaceship.Reset(); enemyLasers.clear(); powerups.clear(); LevelManager::StartPhase(*this,hi,1);
            } else if (currentMenuOption==1) state=GameState::LEVEL_SELECT;
            else if (currentMenuOption==2) state=GameState::INSTRUCTIONS;
            else if (currentMenuOption==3) { state=GameState::SHIP_SELECT; currentShipSelectOption=0; }
            else if (currentMenuOption==4) { stateBeforeSettings=GameState::MENU; state=GameState::SETTINGS; currentSettingsOption=0; }
        }
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int cy=GetScreenHeight()/2;
            const char* opts[5]={"CHOI GAME","CHON MAN","HUONG DAN","CHIEN HAM","CAI DAT"};
            for (int i=0;i<5;i++) {
                int txtW=MeasureText(opts[i],32); int txtX=GetScreenWidth()/2-txtW/2; int txtY=cy-30+i*50;
                Rectangle btn={(float)(txtX-15),(float)(txtY-5),(float)(txtW+30),42.0f};
                if (CheckCollisionPointRec(GetMousePosition(),btn)) {
                    if(i==0){int hi=1;for(int j=0;j<3;j++)if(levels[j].isUnlocked)hi=j+1;
                        isGameActive=true;state=GameState::PLAYING;score=0;godMode=false;
                        spaceship.Reset();enemyLasers.clear();powerups.clear();LevelManager::StartPhase(*this,hi,1);}
                    else if(i==1)state=GameState::LEVEL_SELECT;
                    else if(i==2)state=GameState::INSTRUCTIONS;
                    else if(i==3){state=GameState::SHIP_SELECT;currentShipSelectOption=0;}
                    else if(i==4){stateBeforeSettings=GameState::MENU;state=GameState::SETTINGS;currentSettingsOption=0;}
                }
            }
        }
        {int cy=GetScreenHeight()/2; const char* opts[5]={"CHOI GAME","CHON MAN","HUONG DAN","CHIEN HAM","CAI DAT"};
        for(int i=0;i<5;i++){int txtW=MeasureText(opts[i],32); int txtX=GetScreenWidth()/2-txtW/2; int txtY=cy-30+i*50;
            Rectangle btn={(float)(txtX-15),(float)(txtY-5),(float)(txtW+30),42.0f};
            if(CheckCollisionPointRec(GetMousePosition(),btn))currentMenuOption=i;}}
        return;
    }

    // ---- SHIP SELECT ----
    if (state==GameState::SHIP_SELECT) {
        if (IsKeyPressed(KEY_LEFT)){currentShipSelectOption--;if(currentShipSelectOption<0)currentShipSelectOption=2;}
        if (IsKeyPressed(KEY_RIGHT)){currentShipSelectOption++;if(currentShipSelectOption>2)currentShipSelectOption=0;}
        if (IsKeyPressed(KEY_ENTER)) {
            const char* paths[3]={"Graphics/spaceship.png","Graphics/spaceship 1.png","Graphics/spaceship 2.png"};
            spaceship.ChangeShip(paths[currentShipSelectOption]); state=GameState::MENU;
        }
        if (IsKeyPressed(KEY_ESCAPE)) state=GameState::MENU;

        int cy = GetScreenHeight() / 2;
        int boxSize = 120, spacing = 60, totalW = 3 * boxSize + 2 * spacing;
        int startX = GetScreenWidth()/2 - totalW/2, startY = cy - 60;
        for(int i=0;i<3;i++) {
            Rectangle btn={(float)(startX + i*(boxSize+spacing)), (float)startY, (float)boxSize, (float)boxSize};
            if(CheckCollisionPointRec(GetMousePosition(),btn)) {
                currentShipSelectOption=i;
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    const char* paths[3]={"Graphics/spaceship.png","Graphics/spaceship 1.png","Graphics/spaceship 2.png"};
                    spaceship.ChangeShip(paths[currentShipSelectOption]); state=GameState::MENU;
                }
            }
        }
        return;
    }

    // ---- LEVEL SELECT ----
    if (state==GameState::LEVEL_SELECT) {
        if (IsKeyPressed(KEY_ESCAPE)){state=GameState::MENU;return;}
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            int boxSize=100,spacing=30,totalWidth=3*boxSize+2*spacing;
            int startX=(GetScreenWidth()-totalWidth)/2, y=GetScreenHeight()/2-80;
            for(int i=0;i<3;i++){
                Rectangle rect={(float)(startX+i*(boxSize+spacing)),(float)y,(float)boxSize,(float)boxSize};
                if(CheckCollisionPointRec(GetMousePosition(),rect)&&levels[i].isUnlocked){
                    isGameActive=true;state=GameState::PLAYING;score=0;godMode=false;
                    spaceship.Reset();enemyLasers.clear();powerups.clear();LevelManager::StartPhase(*this,i+1,1);return;
                }
            }
        }
        return;
    }

    // ---- INSTRUCTIONS ----
    if (state==GameState::INSTRUCTIONS) {
        if(IsKeyPressed(KEY_ENTER)||IsKeyPressed(KEY_ESCAPE)||IsMouseButtonPressed(MOUSE_LEFT_BUTTON))state=GameState::MENU; return;
    }

    // ---- GAME OVER ----
    if (state==GameState::GAME_OVER) {
        if(IsKeyPressed(KEY_ENTER)||IsKeyPressed(KEY_ESCAPE)){isGameActive=false;state=GameState::MENU;bgY=0.0f;}
        int cy=GetScreenHeight()/2; const char* btnText=">> ESC hoac ENTER de ve MENU <<";
        int btnSize=24, btnW=MeasureText(btnText,btnSize);
        Rectangle btn={(float)(GetScreenWidth()/2-btnW/2-18), (float)(cy+60), (float)(btnW+36), (float)(btnSize+20)};
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(),btn)) {
            isGameActive=false;state=GameState::MENU;bgY=0.0f;
        }
        return;
    }

    // ---- VICTORY ----
    if (state==GameState::VICTORY) {
        if(IsKeyPressed(KEY_ESCAPE)){isGameActive=false;state=GameState::MENU;bgY=0.0f;}
        if(IsKeyPressed(KEY_ENTER)){
            if(currentLevel<3){isGameActive=true;state=GameState::PLAYING;score=0;godMode=false;
                spaceship.Reset();enemyLasers.clear();powerups.clear();LevelManager::StartPhase(*this,currentLevel+1,1);}
            else{isGameActive=false;state=GameState::MENU;bgY=0.0f;}
        }
        int cy=GetScreenHeight()/2; 
        const char* btnText = (currentLevel < 3) ? ">> ENTER: MAN TIEP THEO | ESC: MENU <<" : ">> ENTER / ESC de ve MENU <<";
        int btnSize=24, btnW2=MeasureText(btnText,btnSize);
        Rectangle btn={(float)(GetScreenWidth()/2-btnW2/2-18), (float)(cy+122), (float)(btnW2+36), (float)(btnSize+20)};
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(),btn)) {
            if(currentLevel<3){isGameActive=true;state=GameState::PLAYING;score=0;godMode=false;
                spaceship.Reset();enemyLasers.clear();powerups.clear();LevelManager::StartPhase(*this,currentLevel+1,1);}
            else{isGameActive=false;state=GameState::MENU;bgY=0.0f;}
        }
        return;
    }

    // ---- PAUSE BUTTON ----
    Rectangle pauseBtn={(float)GetScreenWidth()-50,10,40,40};
    bool clickedPause=IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&CheckCollisionPointRec(GetMousePosition(),pauseBtn);
    bool rightClickPause=IsMouseButtonPressed(MOUSE_RIGHT_BUTTON);

    if (state==GameState::PLAYING) {
        if(IsKeyPressed(KEY_P)||IsKeyPressed(KEY_ESCAPE)||clickedPause||rightClickPause)
            {state=GameState::PAUSED;currentPauseOption=0;return;}
    } else if (state==GameState::PAUSED) {
        if(IsKeyPressed(KEY_P)||IsKeyPressed(KEY_ESCAPE)||clickedPause||rightClickPause){state=GameState::PLAYING;}
        else if(IsKeyPressed(KEY_UP)){currentPauseOption--;if(currentPauseOption<0)currentPauseOption=3;}
        else if(IsKeyPressed(KEY_DOWN)){currentPauseOption++;if(currentPauseOption>3)currentPauseOption=0;}
        else if(IsKeyPressed(KEY_ENTER)){
            if(currentPauseOption==0)state=GameState::PLAYING;
            else if(currentPauseOption==1){isGameActive=true;state=GameState::PLAYING;score=0;godMode=false;
                spaceship.Reset();enemyLasers.clear();powerups.clear();LevelManager::StartPhase(*this,currentLevel,1);}
            else if(currentPauseOption==2){stateBeforeSettings=GameState::PAUSED;state=GameState::SETTINGS;currentSettingsOption=0;}
            else if(currentPauseOption==3){isGameActive=false;state=GameState::MENU;menuBgY=bgY;}
        }
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            int cy=GetScreenHeight()/2; const char* opts[4]={"CHOI TIEP","CHOI LAI","CAI DAT","TRO LAI"};
            for(int i=0;i<4;i++){
                int fw=24,tw=MeasureText(opts[i],fw),tx=GetScreenWidth()/2-tw/2,ty=cy+10+i*45;
                Rectangle btn={(float)(tx-15),(float)(ty-5),(float)(tw+30),(float)(fw+10)};
                if(CheckCollisionPointRec(GetMousePosition(),btn)){
                    if(i==0)state=GameState::PLAYING;
                    else if(i==1){isGameActive=true;state=GameState::PLAYING;score=0;godMode=false;
                        spaceship.Reset();enemyLasers.clear();powerups.clear();LevelManager::StartPhase(*this,currentLevel,1);}
                    else if(i==2){stateBeforeSettings=GameState::PAUSED;state=GameState::SETTINGS;currentSettingsOption=0;}
                    else if(i==3){isGameActive=false;state=GameState::MENU;menuBgY=bgY;}
                }
            }
        }
        // Hover highlight
        {int cy=GetScreenHeight()/2; const char* opts[4]={"CHOI TIEP","CHOI LAI","CAI DAT","TRO LAI"};
        for(int i=0;i<4;i++){int fw=24,tw=MeasureText(opts[i],fw),tx=GetScreenWidth()/2-tw/2,ty=cy+10+i*45;
            Rectangle btn={(float)(tx-15),(float)(ty-5),(float)(tw+30),(float)(fw+10)};
            if(CheckCollisionPointRec(GetMousePosition(),btn))currentPauseOption=i;}}
        return;
    } else if (state==GameState::SETTINGS) {
        if(IsKeyPressed(KEY_UP)){currentSettingsOption--;if(currentSettingsOption<0)currentSettingsOption=2;}
        else if(IsKeyPressed(KEY_DOWN)){currentSettingsOption++;if(currentSettingsOption>2)currentSettingsOption=0;}
        else if(IsKeyPressed(KEY_LEFT)){
            if(currentSettingsOption==0){currentVolume-=10;if(currentVolume<0)currentVolume=0;}
            else if(currentSettingsOption==1){StopBGM();currentBgm--;if(currentBgm<0)currentBgm=2;PlayBGM();}
        } else if(IsKeyPressed(KEY_RIGHT)){
            if(currentSettingsOption==0){currentVolume+=10;if(currentVolume>100)currentVolume=100;}
            else if(currentSettingsOption==1){StopBGM();currentBgm++;if(currentBgm>2)currentBgm=0;PlayBGM();}
        } else if(IsKeyPressed(KEY_ENTER)||IsKeyPressed(KEY_ESCAPE)){
            if(currentSettingsOption==2||IsKeyPressed(KEY_ESCAPE))state=stateBeforeSettings;}
        
        int cy=GetScreenHeight()/2, y2=cy-80;
        const char* volText = TextFormat("Am luong: < %d%% >", currentVolume);
        const char* bgmText = TextFormat("Nhac nen: < Ban nhac %d >", currentBgm + 1);
        const char* opts[3] = {volText, bgmText, "QUAY LAI"};
        for(int i=0;i<3;i++) {
            int txtW = MeasureText(opts[i], 32);
            int txtX = GetScreenWidth()/2 - txtW/2;
            int txtY = y2 + i * 60;
            Rectangle btn = {(float)(txtX-20), (float)(txtY-5), (float)(txtW+40), 42.0f};
            if(CheckCollisionPointRec(GetMousePosition(),btn)) {
                currentSettingsOption = i;
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if(i==0) { currentVolume+=10; if(currentVolume>100)currentVolume=0; }
                    else if(i==1) { StopBGM(); currentBgm++; if(currentBgm>2)currentBgm=0; PlayBGM(); }
                    else if(i==2) state=stateBeforeSettings;
                }
            }
        }
        return;
    }

    // ---- PLAYING — Di chuyen & ban ----
    bool isAltHeld=IsKeyDown(KEY_LEFT_ALT)||IsKeyDown(KEY_RIGHT_ALT);
    if(!isAltHeld) spaceship.MoveTo(GetMousePosition());
    if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)) spaceship.FireLaser();
    if(IsKeyPressed(KEY_Q)&&(spaceship.inventory[0]>0||godMode)){if(!godMode)spaceship.inventory[0]--;spaceship.ApplyHeal();StopSound(sfxPowerup);PlaySound(sfxPowerup);}
    if(IsKeyPressed(KEY_W)&&(spaceship.inventory[1]>0||godMode)){if(!godMode)spaceship.inventory[1]--;spaceship.ApplyRapidFire();StopSound(sfxPowerup);PlaySound(sfxPowerup);}
    if(IsKeyPressed(KEY_E)&&(spaceship.inventory[2]>0||godMode)){if(!godMode)spaceship.inventory[2]--;spaceship.ApplyShield();StopSound(sfxPowerup);PlaySound(sfxPowerup);}
    if(IsKeyPressed(KEY_R)&&(spaceship.inventory[3]>0||godMode)){if(!godMode)spaceship.inventory[3]--;UseBomb();}
}
