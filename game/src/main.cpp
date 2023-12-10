#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// mtv points from rect to circle
inline bool CircleRect(Vector2 circle, float radius, Vector2 rect, Vector2 extents, Vector2* mtv = nullptr)
{
    Vector2 nearest = {
        Clamp(circle.x, rect.x - extents.x, rect.x + extents.x),
        Clamp(circle.y, rect.y - extents.y, rect.y + extents.y),
    };

    float distance = Distance(circle, nearest);
    bool collision = distance <= radius;
    if (collision && mtv != nullptr)
        *mtv = Normalize(circle - rect) * (radius - distance);
    return collision;
}

int main(void)
{
    InitWindow(1280, 720, "Game");
    SetTargetFPS(60);

    Vector2 circle = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    while (!WindowShouldClose())
    {
        const float r = 50.0f;
        const float w = 60.0f;
        const float h = 40.0f;
        const Vector2 rect = GetMousePosition();

        Vector2 mtv{};
        Color color = CircleRect(circle, r, rect, { w * 0.5f, h * 0.5f }, &mtv) ? RED : GREEN;
        circle = circle + mtv;

        Vector2 nearest = {
            Clamp(circle.x, rect.x - w * 0.5f, rect.x + w * 0.5f),
            Clamp(circle.y, rect.y - h * 0.5f, rect.y + h * 0.5f),
        };

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
