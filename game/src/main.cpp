#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);

    Vector2 center{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    Vector2 direction{ 1.0f, 0.0f };
    float length = 500.0f;
    float thickness = 5.0f;

    Vector2 start1{ SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.25f };
    Vector2 end1{ SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.75f };

    while (!WindowShouldClose())
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            start1 = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            end1 = GetMousePosition();

        float rotation = 250.0f * DEG2RAD * GetFrameTime();
        if (IsKeyDown(KEY_E))
            direction = Rotate(direction, rotation);
        if (IsKeyDown(KEY_Q))
            direction = Rotate(direction, -rotation);

        Vector2 start2 = center + direction * length * 0.5f;
        Vector2 end2 = center + direction * length * -0.5f;

        // Tunnel into the CheckCollisionLines function to uncover the math behind line-line intersection!
        Vector2 poi{};
        bool collision = CheckCollisionLines(start1, end1, start2, end2, &poi);
        Color color = collision ? RED : GREEN;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawLineEx(start1, end1, thickness, color);
        DrawLineEx(start2, end2, thickness, color);
        if (collision)
            DrawCircleV(poi, 10.0f, GRAY);
        DrawText("Left & right click to set line 1 start & end", 10, 10, 20, GRAY);
        DrawText("Hold E & Q to rotate line 2 clockwise/counter-clockwise", 10, 30, 20, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
