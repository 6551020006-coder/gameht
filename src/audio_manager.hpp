#pragma once
#include <raylib.h>

// Định nghĩa các loại hiệu ứng âm thanh bằng Enum cho dễ quản lý
enum class SFXType {
    LASER,
    EXPLODE,
    ENEMY_LASER,
    HIT,
    POWERUP,
    HURT,
    BOSS,
    WIN
};

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    void PlayMusic(int index);
    void StopMusic();
    void UpdateMusic();
    void PlaySFX(SFXType type);
    void SetVolume(int volume); // Nhận giá trị từ 0 đến 100
    int GetVolume() const { return currentVolume; }

private:
    Music bgm[3];
    int currentBgm;
    bool bgmPlaying;

    Sound sfxLaser;
    Sound sfxExplode;
    Sound sfxEnemyLaser;
    Sound sfxHit;
    Sound sfxPowerup;
    Sound sfxHurt;
    Sound sfxBoss;
    Sound sfxWin;

    int currentVolume;
};