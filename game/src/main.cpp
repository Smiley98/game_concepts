#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);

    const float x1 = SCREEN_WIDTH * 0.25f;
    const float x2 = SCREEN_WIDTH * 0.75f;
    const float y = SCREEN_HEIGHT * 0.5f;

    while (!WindowShouldClose())
    {
        const float t = fmodf(GetTime(), 1.0f);
        const float x = Lerp(x1, x2, t);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawLine(x1, y, x2, y, GRAY);
        DrawCircle(x1, y, 25.0f, GRAY);
        DrawCircle(x2, y, 25.0f, GRAY);
        DrawCircle(x, y, 25.0f, BLUE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}