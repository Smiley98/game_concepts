#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// mtv points from rect to circle
inline bool CircleRect(Vector2 circle, float radius, Vector2 rect, Vector2 extents, Vector2* mtv = nullptr)
{
    Vector2 nearest = Clamp(circle, rect - extents, rect + extents);
    return Distance(circle, nearest) <= radius;
    // Optimization -- return DistanceSqr(circle, nearest) <= radius * radius;
}

int main(void)
{
    InitWindow(1280, 720, "Game");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        const float r = 50.0f;
        const float w = 60.0f;
        const float h = 40.0f;
        const Vector2 circle = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
        const Vector2 rect = GetMousePosition();
        Vector2 extents = { w * 0.5f, h * 0.5f };
        Vector2 nearest = Clamp(circle, rect - extents, rect + extents);
        Color color = CircleRect(circle, r, rect, extents) ? RED : GREEN;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(circle, r, color);
        DrawRectangleV({ rect.x - w * 0.5f, rect.y - h * 0.5f }, { w, h }, color);
        DrawLineEx(circle, nearest, 5.0f, GOLD);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
