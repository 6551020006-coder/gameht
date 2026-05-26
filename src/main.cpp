#include <raylib.h>
#include "game.hpp"
#include <stdexcept>

int main()
{
    Color grey = {29,29,27,255};

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "CHIEN TRANH GIUA CAC VI SAO");
    InitAudioDevice();
    TraceLog(LOG_INFO, "Audio ready: %d", IsAudioDeviceReady());
    SetExitKey(0); // Tat chuc nang tu dong thoat bang phim ESC
    SetTargetFPS(60);

    try {
        Game game;

        while (WindowShouldClose() == false)
        {
            game.Update();

            BeginDrawing();
            ClearBackground(grey);
            game.Draw();
            EndDrawing();
        }
    } catch (const std::exception& e) {
        // Bat loi tu Game::Game() hoac Spaceship::Spaceship()
        TraceLog(LOG_FATAL, "GAME BI DUNG DO LOI NGOAI LE: %s", e.what());
    }

    CloseAudioDevice();
    CloseWindow();
}