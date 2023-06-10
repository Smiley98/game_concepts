#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    rlImGuiSetup(true);
    InitAudioDevice();

    bool musicPaused = false;
    Music music = LoadMusicStream("../game/assets/audio/ncs_time_leap_aero_chord.mp3");
    Sound sound = LoadSound("../game/assets/audio/yay.ogg");

    bool useGUI = false;
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        if (musicPaused)
            PauseMusicStream(music);
        else
            PlayMusicStream(music);

        const float tt = GetTime();
        unsigned char r = (cosf(tt + PI * 0.33f) * 0.5f + 0.5f) * 255.0f;
        unsigned char g = (cosf(tt + PI * 0.67f) * 0.5f + 0.5f) * 255.0f;
        unsigned char b = (cosf(tt + PI * 1.00f) * 0.5f + 0.5f) * 255.0f;
        unsigned char a = 255;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, { r, g, b, a });

        if (IsKeyPressed(KEY_GRAVE)) useGUI = !useGUI;
        if (useGUI)
        {
            rlImGuiBegin();

            if (ImGui::Button("Play Sound"))
                PlaySound(sound);

            if (musicPaused)
            {
                if (ImGui::Button("Play Music"))
                    musicPaused = false;
            }
            else
            {
                if (ImGui::Button("Pause Music"))
                    musicPaused = true;
            }

            if (ImGui::Button("Restart Music"))
                SeekMusicStream(music, 0.0f);

            rlImGuiEnd();
        }

        DrawFPS(10, 10);
        DrawText("Press ~ to open/close GUI", 10, 30, 20, GRAY);
        EndDrawing();
    }

    UnloadSound(sound);
    UnloadMusicStream(music);

    CloseAudioDevice();
    rlImGuiShutdown();
    CloseWindow();

    return 0;
}