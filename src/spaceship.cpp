#include "spaceship.hpp"
#include <stdexcept>

const char* Spaceship::selectedShipPath = "Graphics/spaceship.png";

Spaceship::Spaceship()
{
    try {
        // Load anh va xoa nen de lam trong suot
        Image img = LoadImage(selectedShipPath);
        if (img.data == nullptr) {
            throw std::runtime_error(TextFormat("Loi: Khong the load file %s", selectedShipPath));
        }
        // Xoa cac pixel mau toi/nen (gan den) thanh trong suot
        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                Color pixel = GetImageColor(img, x, y);
                // Neu pixel qua toi (nen den hoac gan den) -> lam trong suot
                if (pixel.r < 30 && pixel.g < 30 && pixel.b < 30) {
                    ImageDrawPixel(&img, x, y, BLANK);
                }
            }
        }
        image = LoadTextureFromImage(img);
        UnloadImage(img);
        position.x = (GetScreenWidth()  - image.width)  / 2.0f;
        position.y = GetScreenHeight() - image.height - 100;
        lastFireTime = 0.0;

        // Load am thanh tu file .wav (spaceship tu phat tieng laser cua no)
        laserSound = LoadSound("Sounds/laser.wav");
        if (laserSound.stream.buffer == nullptr) {
            throw std::runtime_error("Loi: Khong the load file Sounds/laser.wav");
        }
        SetSoundVolume(laserSound, 0.8f);
    } catch (const std::exception& e) {
        TraceLog(LOG_ERROR, "NGOAI LE (Spaceship): %s", e.what());
        throw; // Nem tiep de main.cpp bat
    }

    // Trang thai mac dinh
    hp                = 50;
    shieldActive      = false;
    rapidFireActive   = false;
    shieldTimer       = 0.0f;
    rapidFireTimer    = 0.0f;
    invulnerableTimer = 0.0f;
    for (int i = 0; i < 4; i++) inventory[i] = 0;
}

Spaceship::~Spaceship() {
    UnloadTexture(image);
    UnloadSound(laserSound);
}

void Spaceship::Draw() {
    // Ve hieu ung nhap nhay khi bat tu
    if (invulnerableTimer > 0.0f) {
        // Nhap nhay moi 0.1 giay
        int frame = (int)(invulnerableTimer * 10.0f);
        if (frame % 2 == 0)
            DrawTextureV(image, position, Color{255, 255, 255, 180});
        // Bo qua frame le (tao hieu ung nhap nhay)
    } else {
        DrawTextureV(image, position, WHITE);
    }

    // Ve khien neu dang kich hoat
    if (shieldActive) {
        float cx = position.x + image.width  / 2.0f;
        float cy = position.y + image.height / 2.0f;
        float r  = (float)(image.width > image.height ? image.width : image.height) / 2.0f + 8.0f;
        // Hieu ung khien nhap nhay theo thoi gian
        float alpha = 120.0f + sinf((float)GetTime() * 5.0f) * 80.0f;
        DrawCircleLines((int)cx, (int)cy, r,      Color{80, 180, 255, (unsigned char)alpha});
        DrawCircleLines((int)cx, (int)cy, r + 3,  Color{80, 180, 255, (unsigned char)(alpha / 2)});
    }
}

void Spaceship::Update() {
    float dt = GetFrameTime();

    if (shieldActive) {
        shieldTimer -= dt;
        if (shieldTimer <= 0.0f) { shieldActive = false; shieldTimer = 0.0f; }
    }
    if (rapidFireActive) {
        rapidFireTimer -= dt;
        if (rapidFireTimer <= 0.0f) { rapidFireActive = false; rapidFireTimer = 0.0f; }
    }
    if (invulnerableTimer > 0.0f) {
        invulnerableTimer -= dt;
        if (invulnerableTimer < 0.0f) invulnerableTimer = 0.0f;
    }
}

void Spaceship::MoveTo(Vector2 target) {
    // Tam phi thuyen = tam con tro chuot
    float cx = target.x - image.width  / 2.0f;
    float cy = target.y - image.height / 2.0f;

    // Gioi han trong vung man hinh
    float margin = 20.0f;
    if (cx < margin) cx = margin;
    if (cx > GetScreenWidth()  - image.width  - margin) cx = GetScreenWidth()  - image.width  - margin;
    if (cy < margin) cy = margin;  // Cho phep bay len tan cung man hinh
    if (cy > GetScreenHeight() - image.height - 10) cy = GetScreenHeight() - image.height - 10;

    // Noi suy mo (lerp) de phi thuyen di chuyen muot
    float speed = 18.0f * GetFrameTime();
    position.x += (cx - position.x) * speed * 60.0f * GetFrameTime();
    position.y += (cy - position.y) * speed * 60.0f * GetFrameTime();
}

void Spaceship::FireLaser()
{
    float interval = rapidFireActive ? 0.08f : 0.15f;

    if (GetTime() - lastFireTime >= interval) {
        if (rapidFireActive) {
            lasers.push_back(Laser({position.x + image.width / 2.0f - 2, position.y}, -18.0f));
            lasers.push_back(Laser({position.x + image.width / 2.0f - 12, position.y + 10}, -18.0f, Color{255, 200, 80, 255}));
            lasers.push_back(Laser({position.x + image.width / 2.0f + 8,  position.y + 10}, -18.0f, Color{255, 200, 80, 255}));
        } else {
            lasers.push_back(Laser({position.x + image.width / 2.0f - 2, position.y}, -15.0f));
        }
        
        // Hieu ung giat lui (recoil) - Tang them theo yeu cau
        float recoilAmount = rapidFireActive ? 7.0f : 15.0f;
        position.y += recoilAmount;
        
        PlaySound(laserSound);
        lastFireTime = GetTime();
    }
}
void Spaceship::Reset()
{
    position.x = (GetScreenWidth()  - image.width)  / 2.0f;
    position.y = GetScreenHeight() - image.height - 100;
    lasers.clear();

    hp                = 50;
    shieldActive      = false;
    rapidFireActive   = false;
    shieldTimer       = 0.0f;
    rapidFireTimer    = 0.0f;
    invulnerableTimer = 0.0f;
    for (int i = 0; i < 4; i++) inventory[i] = 0;
}

// --- AP DUNG KY NANG ---

void Spaceship::ApplyHeal() {
    hp += 20;
    if (hp > 50) hp = 50;  // Gioi han toi da 50 HP
}

void Spaceship::ApplyShield() {
    shieldActive = true;
    shieldTimer  = 5.0f;  // 5 giay
}

void Spaceship::ApplyRapidFire() {
    rapidFireActive = true;
    rapidFireTimer  = 6.0f;  // 6 giay
}

void Spaceship::ChangeShip(const char* newPath) {
    selectedShipPath = newPath;
    Image img = LoadImage(selectedShipPath);
    if (img.data != nullptr) {
        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                Color pixel = GetImageColor(img, x, y);
                if (pixel.r < 30 && pixel.g < 30 && pixel.b < 30) {
                    ImageDrawPixel(&img, x, y, BLANK);
                }
            }
        }
        UnloadTexture(image);
        image = LoadTextureFromImage(img);
        UnloadImage(img);
    }
}
