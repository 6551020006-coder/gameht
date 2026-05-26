#include "audio_manager.hpp"

AudioManager::AudioManager() {
    currentVolume = 50;
    currentBgm = -1;
    bgmPlaying = false;

    // TODO: BẠN HÃY THAY ĐỔI CÁC ĐƯỜNG DẪN DƯỚI ĐÂY CHO ĐÚNG VỚI FILE CỦA BẠN
    bgm[0] = LoadMusicStream("Graphics/bgm1.mp3"); // Thay bằng đường dẫn nhạc của bạn
    bgm[1] = LoadMusicStream("Graphics/bgm2.mp3"); 
    bgm[2] = LoadMusicStream("Graphics/bgm3.mp3"); 

    sfxLaser      = LoadSound("Graphics/laser.wav"); // Thay bằng đường dẫn SFX của bạn
    sfxExplode    = LoadSound("Graphics/explode.wav");
    sfxEnemyLaser = LoadSound("Graphics/enemy_laser.wav");
    sfxHit        = LoadSound("Graphics/hit.wav");
    sfxPowerup    = LoadSound("Graphics/powerup.wav");
    sfxHurt       = LoadSound("Graphics/hurt.wav");
    sfxBoss       = LoadSound("Graphics/boss.wav");
    sfxWin        = LoadSound("Graphics/win.wav");

    SetVolume(currentVolume);
}

AudioManager::~AudioManager() {
    for (int i = 0; i < 3; i++) {
        UnloadMusicStream(bgm[i]);
    }
    UnloadSound(sfxLaser);
    UnloadSound(sfxExplode);
    UnloadSound(sfxEnemyLaser);
    UnloadSound(sfxHit);
    UnloadSound(sfxPowerup);
    UnloadSound(sfxHurt);
    UnloadSound(sfxBoss);
    UnloadSound(sfxWin);
}

void AudioManager::PlayMusic(int index) {
    if (index < 0 || index >= 3) return;
    if (currentBgm == index && bgmPlaying) return;

    StopMusic();
    currentBgm = index;
    PlayMusicStream(bgm[currentBgm]);
    bgmPlaying = true;
}

void AudioManager::StopMusic() {
    if (currentBgm >= 0 && currentBgm < 3 && bgmPlaying) {
        StopMusicStream(bgm[currentBgm]);
    }
    bgmPlaying = false;
}

void AudioManager::UpdateMusic() {
    if (currentBgm >= 0 && currentBgm < 3 && bgmPlaying) {
        UpdateMusicStream(bgm[currentBgm]);
    }
}

void AudioManager::PlaySFX(SFXType type) {
    switch (type) {
        case SFXType::LASER:       PlaySound(sfxLaser); break;
        case SFXType::EXPLODE:     PlaySound(sfxExplode); break;
        case SFXType::ENEMY_LASER: PlaySound(sfxEnemyLaser); break;
        case SFXType::HIT:         PlaySound(sfxHit); break;
        case SFXType::POWERUP:     PlaySound(sfxPowerup); break;
        case SFXType::HURT:        PlaySound(sfxHurt); break;
        case SFXType::BOSS:        PlaySound(sfxBoss); break;
        case SFXType::WIN:         PlaySound(sfxWin); break;
    }
}

void AudioManager::SetVolume(int volume) {
    currentVolume = volume;
    float volumeFactor = (float)currentVolume / 100.0f;
    
    // Áp dụng volume cho nhạc và sfx
    for (int i = 0; i < 3; i++) SetMusicVolume(bgm[i], volumeFactor);
    SetSoundVolume(sfxLaser, volumeFactor);
    SetSoundVolume(sfxExplode, volumeFactor);
    SetSoundVolume(sfxEnemyLaser, volumeFactor);
    SetSoundVolume(sfxHit, volumeFactor);
    SetSoundVolume(sfxPowerup, volumeFactor);
    SetSoundVolume(sfxHurt, volumeFactor);
    SetSoundVolume(sfxBoss, volumeFactor);
    SetSoundVolume(sfxWin, volumeFactor);
}