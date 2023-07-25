#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);

    float FoV = 60.0f * DEG2RAD;  // field of view
    float viewDistance = 250.0f;
    Vector2 viewerPosition{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    Vector2 targetPosition{ SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.25f };

    while (!WindowShouldClose())
    {
        Vector2 viewerDirection = Direction(GetTime());

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(viewerPosition, 25.0f, BLUE);
        DrawLineV(viewerPosition, viewerPosition + Rotate(viewerDirection, FoV *  0.5f) * viewDistance, BLUE);
        DrawLineV(viewerPosition, viewerPosition + Rotate(viewerDirection, FoV * -0.5f) * viewDistance, BLUE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}