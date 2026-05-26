#include "ui_manager.hpp"
#include "game.hpp"
#include "boss.hpp"
#include <cmath>

// -------------------------------------------------------
// Helper nội bộ (static local)
// -------------------------------------------------------
static void DrawHeartShape(float cx, float cy, float size, Color color) {
    float r = size * 0.27f;
    DrawCircleV({cx - r * 0.72f, cy - size * 0.08f}, r, color);
    DrawCircleV({cx + r * 0.72f, cy - size * 0.08f}, r, color);
    Vector2 v1 = {cx - size * 0.50f, cy - size * 0.04f};
    Vector2 v2 = {cx + size * 0.50f, cy - size * 0.04f};
    Vector2 v3 = {cx, cy + size * 0.50f};
    DrawTriangle(v2, v1, v3, color);
}

// -------------------------------------------------------
// DRAW MENU
// -------------------------------------------------------
void UIManager::DrawMenu(Game& g) {
    g.DrawScrollingBgPublic(g.bgTexture, g.menuBgY);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0, 0, 0, 160});

    int cy = GetScreenHeight() / 2;
    const char* title = "CHIEN TRANH GIUA CAC VI SAO";
    int titleSize = 72;
    int titleW = MeasureText(title, titleSize);
    DrawText(title, GetScreenWidth() / 2 - titleW / 2 + 4, cy - 200 + 4, titleSize, Color{0, 0, 0, 180});
    DrawText(title, GetScreenWidth() / 2 - titleW / 2, cy - 200, titleSize, Color{80, 200, 255, 255});

    DrawRectangle(GetScreenWidth() / 2 - 160, cy - 120, 320, 2, Color{80, 200, 255, 160});

    // Removed global high score display

    const char* options[5] = {"CHOI GAME", "CHON MAN", "HUONG DAN", "CHIEN HAM", "CAI DAT"};
    for (int i = 0; i < 5; i++) {
        int fontSize = 32;
        int txtW = MeasureText(options[i], fontSize);
        int txtX = GetScreenWidth() / 2 - txtW / 2;
        int txtY = cy - 30 + i * 50;

        if (i == g.currentMenuOption) {
            int bw = txtW + 60, bh = fontSize + 10;
            int bx = GetScreenWidth() / 2 - bw / 2, by = txtY - 5;
            float blink = (sinf(g.menuPulse * 8.0f) + 1.0f) / 2.0f;
            unsigned char alphaBg  = (unsigned char)(80  + blink * 100);
            unsigned char alphaBor = (unsigned char)(150 + blink * 105);
            unsigned char alphaTxt = (unsigned char)(155 + blink * 100);
            DrawRectangleRounded({(float)bx,(float)by,(float)bw,(float)bh}, 0.5f, 6, Color{80,80,200,alphaBg});
            DrawRectangleLinesEx({(float)bx,(float)by,(float)bw,(float)bh}, 2, Color{120,120,255,alphaBor});
            DrawText(options[i], txtX, txtY, fontSize, Color{255,255,255,alphaTxt});
        } else {
            DrawText(options[i], txtX, txtY, fontSize, Color{180,180,180,200});
        }
    }

    const char* footer = "Nhan ENTER hoac CHUOT de xac nhan";
    int fW = MeasureText(footer, 20);
    DrawText(footer, GetScreenWidth() / 2 - fW / 2, GetScreenHeight() - 50, 20, LIGHTGRAY);
}

// -------------------------------------------------------
// DRAW LEVEL SELECT
// -------------------------------------------------------
void UIManager::DrawLevelSelect(Game& g) {
    g.DrawScrollingBgPublic(g.bgTexture, g.menuBgY);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0, 0, 0, 180});

    int cy = GetScreenHeight() / 2;
    const char* title = "CHON MAN CHOI";
    int titleSize = 60;
    int titleW = MeasureText(title, titleSize);
    DrawText(title, GetScreenWidth() / 2 - titleW / 2 + 2, cy - 250 + 2, titleSize, Color{0,0,0,180});
    DrawText(title, GetScreenWidth() / 2 - titleW / 2, cy - 250, titleSize, Color{80,200,255,255});

    int boxSize = 100, spacing = 30;
    int totalWidth = 3 * boxSize + 2 * spacing;
    int startX = (GetScreenWidth() - totalWidth) / 2;
    int y = cy - 80;

    for (int i = 0; i < 3; i++) {
        int x = startX + i * (boxSize + spacing);
        Rectangle rect = {(float)x, (float)y, (float)boxSize, (float)boxSize};
        bool isHover = CheckCollisionPointRec(GetMousePosition(), rect);
        LevelData ld = g.levels[i];

        if (ld.isUnlocked) {
            Color boxColor = isHover ? Color{80,180,255,200} : Color{50,120,200,150};
            DrawRectangleRounded(rect, 0.2f, 8, boxColor);
            DrawRectangleRoundedLinesEx(rect, 0.2f, 8, 2.0f, WHITE);
            const char* num = TextFormat("%d", i + 1);
            int nw = MeasureText(num, 40);
            DrawText(num, x + boxSize/2 - nw/2, y + 15, 40, WHITE);
            for (int s = 0; s < 3; s++) {
                if (s < ld.stars) {
                    Rectangle src = {0,0,(float)g.texStarQuaman.width,(float)g.texStarQuaman.height};
                    Rectangle dst = {(float)(x+20+s*20),(float)(y+70),20.0f,20.0f};
                    DrawTexturePro(g.texStarQuaman, src, dst, {0,0}, 0.0f, WHITE);
                } else {
                    DrawCircle(x+30+s*20, y+80, 8, Color{20,20,20,180});
                }
            }
            
            const char* hsTxt = TextFormat("HS: %d", ld.highScore);
            int hsW = MeasureText(hsTxt, 16);
            DrawText(hsTxt, x + boxSize/2 - hsW/2, y + boxSize + 10, 16, YELLOW);
        } else {
            DrawRectangleRounded(rect, 0.2f, 8, Color{50,50,50,150});
            DrawRectangleRoundedLinesEx(rect, 0.2f, 8, 2.0f, GRAY);
            int nw = MeasureText("X", 40);
            DrawText("X", x + boxSize/2 - nw/2, y + 30, 40, GRAY);
        }
    }

    const char* hint = "Click de chon man, hoac ESC de ve MENU";
    int hintW = MeasureText(hint, 20);
    DrawText(hint, GetScreenWidth() / 2 - hintW / 2, cy + 150, 20, LIGHTGRAY);
}

// -------------------------------------------------------
// DRAW INSTRUCTIONS
// -------------------------------------------------------
void UIManager::DrawInstructions(Game& g) {
    g.DrawScrollingBgPublic(g.bgTexture, g.menuBgY);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0,0,0,200});

    int cy = GetScreenHeight() / 2;
    const char* title = "HUONG DAN CHOI";
    int titleW = MeasureText(title, 40);
    DrawText(title, GetScreenWidth()/2 - titleW/2, cy - 300, 40, Color{80,200,255,255});

    int y = cy - 200;
    DrawText("- Di chuyen: Chuot ", 100, y, 20, WHITE); y += 40;
    DrawText("- Ban laser: Chuot trai", 100, y, 20, WHITE); y += 40;
    DrawText("- Tu dong nhat vat pham vao tui do (goc phai duoi).", 100, y, 20, YELLOW); y += 40;
    DrawText("- Su dung ky nang:", 100, y, 20, WHITE); y += 30;
    DrawText("    + Phim Q: Hoi mau", 100, y, 20, GREEN); y += 30;
    DrawText("    + Phim W: Ban nhanh (3 tia)", 100, y, 20, ORANGE); y += 30;
    DrawText("    + Phim E: Bat khien", 100, y, 20, SKYBLUE); y += 30;
    DrawText("    + Phim R: Dat bom (Tieu diet toan bo man hinh)", 100, y, 20, RED); y += 40;
    DrawText("- Phim M: Bat/Tat nhac", 100, y, 20, LIGHTGRAY); y += 30;
    DrawText("- Phim Shift + G: Bat tu (God Mode)", 100, y, 20, LIGHTGRAY); y += 30;
    DrawText("- Phim P hoac ESC hoac Click nut Pause de TAM DUNG", 100, y, 20, LIGHTGRAY); y += 50;

    const char* exitTxt = ">> Nhan ESC hoac ENTER de quay lai <<";
    int exitW = MeasureText(exitTxt, 20);
    DrawText(exitTxt, GetScreenWidth()/2 - exitW/2, cy + 250, 20, Color{80,255,80,255});
}

// -------------------------------------------------------
// DRAW GAME OVER
// -------------------------------------------------------
void UIManager::DrawGameOver(Game& g) {
    g.DrawScrollingBgPublic(g.bgTexture, g.menuBgY);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0,0,0,170});

    int cy = GetScreenHeight() / 2;
    int heartY = cy - 170, heartSize = 52, heartSpacing = 70;
    int totalW = 3 * heartSize + 2 * (heartSpacing - heartSize);
    int startX = (GetScreenWidth() - totalW) / 2 - heartSize / 2;
    for (int i = 0; i < 3; i++) {
        int hx = startX + i * heartSpacing;
        float flicker = 0.4f + sinf(g.gameOverPulse * 2.0f + i) * 0.15f;
        DrawHeartShape(hx + heartSize*0.5f, heartY + heartSize*0.5f, (float)heartSize, Color{180,20,20,(unsigned char)(flicker*160)});
    }

    const char* goText = "GAME OVER";
    int goSize = 72, goW = MeasureText(goText, goSize);
    DrawText(goText, GetScreenWidth()/2 - goW/2 + 3, cy - 100 + 3, goSize, Color{0,0,0,180});
    float pulse = 0.85f + sinf(g.gameOverPulse * 2.5f) * 0.15f;
    DrawText(goText, GetScreenWidth()/2 - goW/2, cy - 100, goSize,
             Color{(unsigned char)(255*pulse),(unsigned char)(60*pulse),50,255});

    const char* scoreText = TextFormat("Diem so: %d", g.score);
    int sW = MeasureText(scoreText, 28);
    DrawText(scoreText, GetScreenWidth()/2 - sW/2, cy - 10, 28, Color{255,220,80,230});
    DrawRectangle(GetScreenWidth()/2 - 160, cy + 34, 320, 2, Color{200,60,60,160});

    float alpha2 = 180.0f + sinf(g.gameOverPulse) * 70.0f;
    float scale2 = 1.0f + sinf(g.gameOverPulse) * 0.04f;
    const char* btnText = ">> ESC hoac ENTER de ve MENU <<";
    int btnSize = (int)(24 * scale2);
    int btnW = MeasureText(btnText, btnSize);
    int bx = GetScreenWidth()/2 - btnW/2 - 18, by = cy + 60, bw = btnW + 36, bh = btnSize + 20;
    DrawRectangleRounded({(float)bx,(float)by,(float)bw,(float)bh}, 0.4f, 8,
                         Color{180,30,30,(unsigned char)(alpha2*0.35f)});
    DrawRectangleLinesEx({(float)bx,(float)by,(float)bw,(float)bh}, 2,
                         Color{255,80,80,(unsigned char)alpha2});
    DrawText(btnText, GetScreenWidth()/2 - btnW/2, by + 10, btnSize,
             Color{255,255,255,(unsigned char)alpha2});

    const char* quit = "ESC: Thoat";
    int qW = MeasureText(quit, 16);
    DrawText(quit, GetScreenWidth()/2 - qW/2, cy + 174, 16, Color{160,160,160,180});
}

// -------------------------------------------------------
// DRAW VICTORY
// -------------------------------------------------------
void UIManager::DrawVictory(Game& g) {
    g.DrawScrollingBgPublic(g.bgTexture, g.menuBgY);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0,0,0,140});

    int cy = GetScreenHeight() / 2;

    // Confetti sao
    for (int i = 0; i < 30; i++) {
        float t = g.victoryPulse + i * 0.7f;
        float px = 100.0f + (float)(i * 61 % (int)(GetScreenWidth() - 200));
        float py = 60.0f  + (float)(i * 37 % 200);
        float sz = 2.0f + sinf(t * 3.0f + i) * 2.0f;
        float brightness = 0.5f + sinf(t * 4.0f + i * 1.3f) * 0.5f;
        Color starColors[4] = {
            {255,220,60,(unsigned char)(200*brightness)},
            {255,140,20,(unsigned char)(200*brightness)},
            {80,200,255,(unsigned char)(200*brightness)},
            {255,100,200,(unsigned char)(200*brightness)},
        };
        DrawCircle((int)px, (int)py, sz, starColors[i % 4]);
    }

    // Ring
    float ringAlpha = 130.0f + sinf(g.victoryPulse * 2.0f) * 60.0f;
    DrawCircleLines(GetScreenWidth()/2, cy - 130, 90.0f + sinf(g.victoryPulse)*5.0f, Color{255,200,50,(unsigned char)ringAlpha});
    DrawCircleLines(GetScreenWidth()/2, cy - 130, 82.0f + sinf(g.victoryPulse)*5.0f, Color{255,200,50,(unsigned char)(ringAlpha*0.5f)});

    // Cup
    {
        float tp = 1.0f + sinf(g.victoryPulse * 3.0f) * 0.12f;
        unsigned char tg = (unsigned char)(210 * tp);
        Color cupColor = {255, tg, 30, 255};
        Color cupDark  = {(unsigned char)(255*0.7f),(unsigned char)(tg*0.7f),20,255};
        float cx2 = (float)GetScreenWidth() / 2.0f;
        float cupY = (float)cy - 132.0f;
        Vector2 v1={cx2-20,cupY-22}, v2={cx2+20,cupY-22}, v3={cx2+12,cupY+6}, v4={cx2-12,cupY+6};
        DrawTriangle(v2,v1,v4,cupColor); DrawTriangle(v2,v4,v3,cupColor);
        DrawRing({cx2-20,cupY-10},8,12,90,270,16,cupDark);
        DrawRing({cx2+20,cupY-10},8,12,270,450,16,cupDark);
        DrawRectangle((int)(cx2-3),(int)(cupY+6),6,10,cupColor);
        DrawRectangle((int)(cx2-14),(int)(cupY+16),28,5,cupColor);
        DrawRectangle((int)(cx2-16),(int)(cupY+21),32,3,cupDark);
    }

    const char* title = "CHIEN THANG!";
    int titleSize = 64, titleW = MeasureText(title, titleSize);
    float cp = g.victoryPulse * 2.0f;
    DrawText(title, GetScreenWidth()/2 - titleW/2 + 3, cy - 57 + 3, titleSize, Color{0,0,0,180});
    DrawText(title, GetScreenWidth()/2 - titleW/2, cy - 57, titleSize,
             Color{(unsigned char)(200+sinf(cp)*55),(unsigned char)(180+sinf(cp+2.09f)*55),(unsigned char)(50+sinf(cp+4.18f)*50),255});

    const char* scoreText = TextFormat("Diem so: %d", g.score);
    int sW = MeasureText(scoreText, 28);
    DrawText(scoreText, GetScreenWidth()/2 - sW/2, cy + 18, 28, Color{255,230,100,240});

    const char* sub = "Ban da hoan thanh man choi!";
    int subW = MeasureText(sub, 18);
    DrawText(sub, GetScreenWidth()/2 - subW/2, cy + 54, 18, Color{200,240,200,200});

    int earnedStars = 1;
    if (g.spaceship.hp >= 40) earnedStars = 3;
    else if (g.spaceship.hp >= 20) earnedStars = 2;
    for (int s = 0; s < 3; s++) {
        if (s < earnedStars) {
            Rectangle src = {0,0,(float)g.texStarQuaman.width,(float)g.texStarQuaman.height};
            Rectangle dst = {(float)(GetScreenWidth()/2 - 50 + s*35),(float)(cy+65),30.0f,30.0f};
            DrawTexturePro(g.texStarQuaman, src, dst, {0,0}, 0.0f, WHITE);
        } else {
            DrawCircle(GetScreenWidth()/2 - 35 + s*35, cy + 80, 12, Color{20,20,20,180});
        }
    }

    float lineAlpha2 = 160.0f + sinf(g.victoryPulse) * 60.0f;
    DrawRectangle(GetScreenWidth()/2 - 180, cy + 102, 360, 2, Color{255,200,50,(unsigned char)lineAlpha2});

    float alpha2 = 180.0f + sinf(g.victoryPulse) * 70.0f;
    float scale2 = 1.0f + sinf(g.victoryPulse) * 0.04f;
    const char* btnText = (g.currentLevel < 3) ? ">> ENTER: MAN TIEP THEO | ESC: MENU <<" : ">> ENTER / ESC de ve MENU <<";
    int btnSize = (int)(24 * scale2);
    int btnW2 = MeasureText(btnText, btnSize);
    int bx = GetScreenWidth()/2 - btnW2/2 - 18, by = cy + 122, bw = btnW2 + 36, bh = btnSize + 20;
    DrawRectangleRounded({(float)bx,(float)by,(float)bw,(float)bh}, 0.4f, 8, Color{30,140,60,(unsigned char)(alpha2*0.4f)});
    DrawRectangleLinesEx({(float)bx,(float)by,(float)bw,(float)bh}, 2, Color{100,255,120,(unsigned char)alpha2});
    DrawText(btnText, GetScreenWidth()/2 - btnW2/2, by + 10, btnSize, Color{255,255,255,(unsigned char)alpha2});
}

// -------------------------------------------------------
// DRAW PAUSED
// -------------------------------------------------------
void UIManager::DrawPaused(Game& g) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0,0,0,150});
    int cy = GetScreenHeight() / 2;
    const char* title = "TAM DUNG";
    int titleSize = 64, titleW = MeasureText(title, titleSize);
    DrawText(title, GetScreenWidth()/2 - titleW/2 + 3, cy - 100 + 3, titleSize, Color{0,0,0,180});
    DrawText(title, GetScreenWidth()/2 - titleW/2, cy - 100, titleSize, Color{80,200,255,255});

    const char* options[4] = {"CHOI TIEP","CHOI LAI","CAI DAT","TRO LAI"};
    int startY = cy + 10;
    for (int i = 0; i < 4; i++) {
        int fontSize = 24;
        int txtW = MeasureText(options[i], fontSize);
        int txtX = GetScreenWidth()/2 - txtW/2;
        int txtY = startY + i * 45;
        Color textColor = WHITE;
        if (g.currentPauseOption == i) {
            DrawRectangle(txtX - 15, txtY - 5, txtW + 30, fontSize + 10, Color{80,180,255,150});
            textColor = BLACK;
        }
        DrawText(options[i], txtX, txtY, fontSize, textColor);
    }

    const char* hint = "Dung UP/DOWN de chon, ENTER de xac nhan";
    int hintW = MeasureText(hint, 16);
    DrawText(hint, GetScreenWidth()/2 - hintW/2, cy + 160, 16, Color{160,160,160,180});
}

// -------------------------------------------------------
// DRAW SETTINGS
// -------------------------------------------------------
void UIManager::DrawSettings(Game& g) {
    if (g.stateBeforeSettings == GameState::MENU) {
        g.DrawScrollingBgPublic(g.bgTexture, g.menuBgY);
    } else {
        DrawTexture(g.bgTexture, 0, 0, Color{80,80,80,255});
        for (const auto& a : g.asteroids) {
            if (a.active) {
                Rectangle src = {0,0,(float)g.texAsteroid.width,(float)g.texAsteroid.height};
                Rectangle dst = {a.position.x, a.position.y, a.radius*2.5f, a.radius*2.5f};
                DrawTexturePro(g.texAsteroid, src, dst, {a.radius*1.25f,a.radius*1.25f}, a.rotation, Color{150,150,150,255});
            }
        }
    }
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0,0,0,200});

    int cy = GetScreenHeight() / 2;
    const char* title = "CAI DAT";
    int titleW = MeasureText(title, 40);
    DrawText(title, GetScreenWidth()/2 - titleW/2, cy - 200, 40, Color{80,200,255,255});

    int y = cy - 80;
    const char* volText = TextFormat("Am luong: < %d%% >", g.currentVolume);
    const char* bgmText = TextFormat("Nhac nen: < Ban nhac %d >", g.currentBgm + 1);
    const char* options[3] = {volText, bgmText, "QUAY LAI"};

    for (int i = 0; i < 3; i++) {
        int txtW = MeasureText(options[i], 32);
        int txtX = GetScreenWidth()/2 - txtW/2;
        int txtY = y + i * 60;
        if (i == g.currentSettingsOption) {
            float scale = 1.0f + sinf((float)GetTime() * 5.0f) * 0.05f;
            int bw = (int)(txtW * scale) + 60, bh = (int)(42 * scale);
            int bx = GetScreenWidth()/2 - bw/2, by = txtY - 5;
            DrawRectangleRounded({(float)bx,(float)by,(float)bw,(float)bh}, 0.5f, 6, Color{80,80,200,160});
            DrawRectangleLinesEx({(float)bx,(float)by,(float)bw,(float)bh}, 2, Color{120,120,255,200});
            DrawText(options[i], txtX, txtY, 32, Color{255,255,255,255});
        } else {
            DrawText(options[i], txtX, txtY, 32, Color{180,180,180,200});
        }
    }

    const char* hint = "Su dung Phim Trai/Phai de dieu chinh";
    int hintW = MeasureText(hint, 20);
    DrawText(hint, GetScreenWidth()/2 - hintW/2, cy + 150, 20, LIGHTGRAY);
}

// -------------------------------------------------------
// DRAW SHIP SELECT
// -------------------------------------------------------
void UIManager::DrawShipSelect(Game& g) {
    g.DrawScrollingBgPublic(g.bgTexture, g.menuBgY);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0,0,0,180});
    int cy = GetScreenHeight() / 2;
    const char* txt = "CHON CHIEN HAM";
    DrawText(txt, GetScreenWidth()/2 - MeasureText(txt,40)/2, cy - 200, 40, Color{80,200,255,255});

    int boxSize = 120, spacing = 60;
    int totalW = 3 * boxSize + 2 * spacing;
    int startX = GetScreenWidth()/2 - totalW/2;
    int startY = cy - 60;
    const char* shipNames[3] = {"TAU SO 1","TAU SO 2","TAU SO 3"};

    for (int i = 0; i < 3; i++) {
        int x = startX + i * (boxSize + spacing);
        Rectangle rect = {(float)x,(float)startY,(float)boxSize,(float)boxSize};
        Color c = (i == g.currentShipSelectOption) ? Color{80,200,255,255} : Color{180,180,180,150};
        if (i == g.currentShipSelectOption) {
            DrawRectangleRounded(rect, 0.2f, 8, Color{80,180,255,100});
            DrawRectangleRoundedLinesEx(rect, 0.2f, 8, 3.0f, c);
        } else {
            DrawRectangleRounded(rect, 0.2f, 8, Color{50,50,50,150});
            DrawRectangleRoundedLinesEx(rect, 0.2f, 8, 2.0f, c);
        }
        Texture2D t = g.shipSelectTextures[i];
        float scale = 1.0f;
        if (t.width  > boxSize - 20) scale = (float)(boxSize - 20) / t.width;
        if (t.height * scale > boxSize - 20) scale = (float)(boxSize - 20) / t.height;
        Vector2 pos = {x + boxSize/2.0f - (t.width*scale)/2.0f, startY + boxSize/2.0f - (t.height*scale)/2.0f};
        DrawTextureEx(t, pos, 0.0f, scale, WHITE);
        int nw = MeasureText(shipNames[i], 20);
        DrawText(shipNames[i], x + boxSize/2 - nw/2, startY + boxSize + 20, 20, c);
    }

    const char* hint = "Su dung Phim Trai/Phai de chon | ENTER de xac nhan | ESC de huy";
    int hintW = MeasureText(hint, 20);
    DrawText(hint, GetScreenWidth()/2 - hintW/2, GetScreenHeight() - 50, 20, LIGHTGRAY);
}

// -------------------------------------------------------
// DRAW HUD (in-game)
// -------------------------------------------------------
void UIManager::DrawHUD(Game& g) {
    DrawText(TextFormat("Man: %d", g.currentLevel), 10, 10, 20, WHITE);
    DrawText(TextFormat("Score: %d", g.score), 10, 35, 20, WHITE);
    DrawText(TextFormat("High Score: %d", g.levels[g.currentLevel - 1].highScore), 10, 58, 18, RED);
    int remaining = g.spawnQueue - g.spawnedCount;
    DrawText(TextFormat("Enemies: %d", remaining), 10, 80, 20, WHITE);

    int hpBarW = 150, hpBarH = 20, hpBarX = 10, hpBarY = GetScreenHeight() - 34;
    DrawRectangle(hpBarX, hpBarY, hpBarW, hpBarH, Color{80,80,80,180});
    DrawRectangleLines(hpBarX, hpBarY, hpBarW, hpBarH, WHITE);
    float hpRatio = (float)g.spaceship.hp / 50.0f;
    if (hpRatio < 0) hpRatio = 0; if (hpRatio > 1) hpRatio = 1;
    Color hpColor = GREEN;
    if (hpRatio < 0.3f) hpColor = RED;
    else if (hpRatio < 0.6f) hpColor = YELLOW;
    DrawRectangle(hpBarX+1, hpBarY+1, (int)((hpBarW-2)*hpRatio), hpBarH-2, hpColor);
    DrawText(TextFormat("HP: %d / 50", g.spaceship.hp), hpBarX + hpBarW + 10, hpBarY + 2, 20, WHITE);

    if (g.godMode) {
        float gp = (float)GetTime() * 4.0f;
        unsigned char ga = (unsigned char)(180 + sinf(gp) * 75);
        unsigned char gy = (unsigned char)(200 + sinf(gp*1.3f) * 55);
        const char* godText = "\xe2\x9a\xa1 GOD MODE";
        int gw = MeasureText(godText, 18);
        DrawRectangleRounded({(float)(GetScreenWidth()/2 - gw/2 - 10),84,(float)(gw+20),26}, 0.5f, 6, Color{60,40,0,(unsigned char)(ga*0.5f)});
        DrawRectangleLinesEx({(float)(GetScreenWidth()/2 - gw/2 - 10),84,(float)(gw+20),26}, 1, Color{255,gy,0,ga});
        DrawText(godText, GetScreenWidth()/2 - gw/2, 88, 18, Color{255,gy,30,ga});
    }

    int iconSize = 40, marginR = 10, iconY = GetScreenHeight() - iconSize - 10;
    Texture2D texs[4] = {g.texHeal, g.texRapidFire, g.texShield, g.texBomb};
    const char* keys[4] = {"Q","W","E","R"};
    Color activeColors[4] = {GREEN, YELLOW, SKYBLUE, RED};
    for (int i = 0; i < 4; i++) {
        int ix = GetScreenWidth() - marginR - (4-i) * (iconSize + 8);
        int count = g.spaceship.inventory[i];
        bool hasItem = (count > 0 || g.godMode);
        Color tint = hasItem ? WHITE : Color{255,255,255,80};
        Color borderColor = hasItem ? activeColors[i] : GRAY;
        DrawTexturePro(texs[i], {0,0,(float)texs[i].width,(float)texs[i].height},
                       {(float)ix,(float)iconY,(float)iconSize,(float)iconSize}, {0,0}, 0, tint);
        DrawRectangleLinesEx({(float)ix,(float)iconY,(float)iconSize,(float)iconSize}, 1, borderColor);
        DrawRectangle(ix, iconY-14, 14, 14, BLACK);
        DrawText(keys[i], ix+3, iconY-12, 10, WHITE);
        if (hasItem) {
            DrawCircle(ix + iconSize, iconY, g.godMode ? 10.0f : 8.0f, RED);
            if (g.godMode) DrawText("oo", ix+iconSize-5, iconY-4, 10, WHITE);
            else DrawText(TextFormat("%d", count), ix+iconSize-3, iconY-4, 10, WHITE);
        }
    }

    if (g.spaceship.rapidFireActive) {
        int ix = GetScreenWidth() - marginR - 3*(iconSize+8);
        DrawText(TextFormat("%.0fs", g.spaceship.rapidFireTimer), ix+20, iconY-16, 14, YELLOW);
    }
    if (g.spaceship.shieldActive) {
        int ix = GetScreenWidth() - marginR - 2*(iconSize+8);
        DrawText(TextFormat("%.0fs", g.spaceship.shieldTimer), ix+20, iconY-16, 14, SKYBLUE);
    }

    Rectangle pauseBtn = {(float)GetScreenWidth()-50, 10, 40, 40};
    bool isHover = CheckCollisionPointRec(GetMousePosition(), pauseBtn);
    Color btnColor = isHover ? Color{255,255,255,150} : Color{255,255,255,80};
    DrawRectangleRounded(pauseBtn, 0.3f, 4, btnColor);
    DrawRectangleLinesEx(pauseBtn, 2, WHITE);
    DrawRectangle((int)pauseBtn.x+12, (int)pauseBtn.y+10, 6, 20, WHITE);
    DrawRectangle((int)pauseBtn.x+22, (int)pauseBtn.y+10, 6, 20, WHITE);
}

// -------------------------------------------------------
// DRAW HEALTH BAR (enemy)
// -------------------------------------------------------
void UIManager::DrawHealthBar(Game& g, const Enemy& enemy) {
    if (!enemy.alive) return;
    Rectangle rect = enemy.GetRect();

    int maxHp = 1;
    if      (enemy.type == EnemyType::SCOUT)      maxHp = 1;
    else if (enemy.type == EnemyType::MINI_BOSS)  maxHp = 25;
    else if (enemy.type == EnemyType::BOSS)        maxHp = 60;
    else if (enemy.type == EnemyType::GRAND_BOSS)  maxHp = 150;
    else if (enemy.type == EnemyType::FINAL_BOSS)  maxHp = 200;

    if (enemy.type == EnemyType::FINAL_BOSS) {
        int barW = GetScreenWidth()/2, barH = 12;
        int barX = GetScreenWidth()/4, barY = 15;
        DrawRectangle(barX, barY, barW, barH, Color{80,80,80,180});
        float ratio = (float)enemy.hp / (float)maxHp;
        DrawRectangle(barX, barY, (int)(barW*ratio), barH, Color{255,50,50,255});
        DrawRectangleLines(barX, barY, barW, barH, WHITE);
        for (int i = 1; i <= 3; i++) DrawLine(barX + barW*i/4, barY, barX + barW*i/4, barY+barH, WHITE);
        return;
    }

    int barW = (int)rect.width;
    int barH = 4;
    if (enemy.type == EnemyType::MINI_BOSS || enemy.type == EnemyType::BOSS || enemy.type == EnemyType::GRAND_BOSS)
        barW = 60;
    int barX = (int)rect.x + (int)((rect.width - barW) / 2);
    int barY = (int)rect.y - 10;
    DrawRectangle(barX, barY, barW, barH, Color{80,80,80,180});
    float ratio = (float)enemy.hp / (float)maxHp;
    Color col = {(unsigned char)(255*(1.0f-ratio)),(unsigned char)(255*ratio),0,255};
    DrawRectangle(barX, barY, (int)(barW*ratio), barH, col);
}
