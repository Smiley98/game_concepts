#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Rectangle defined by center position + half-extents. Mtv points FROM rectangle TO circle
bool CircleRect(Vector2 circleCenter, float radius, Vector2 rectCenter, Vector2 halfExtents, Vector2* mtv = nullptr)
{
    Vector2 nearest;
    float xMin = rectCenter.x - halfExtents.x;
    float xMax = rectCenter.x + halfExtents.x;
    float yMin = rectCenter.y - halfExtents.y;
    float yMax = rectCenter.y + halfExtents.y;
    nearest.x = fabsf(circleCenter.x - xMin) < fabsf(circleCenter.x - xMax) ? xMin : xMax;
    nearest.y = fabsf(circleCenter.y - yMin) < fabsf(circleCenter.y - yMax) ? yMin : yMax;

    float distance = Distance(circleCenter, nearest);
    bool collision = distance <= radius;
    if (collision && mtv != nullptr)
        *mtv = Normalize(circleCenter - rectCenter) * (radius - distance);
    return collision;
}

int main(void)
{
    InitWindow(1280, 720, "Game");
    SetTargetFPS(60);

    Vector2 circlePosition{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    float radius = 50.0f;

    while (!WindowShouldClose())
    {
        Vector2 mouse = GetMousePosition();
        float rw = 60.0f;
        float rh = 40.0f;

        Vector2 mtv;
        bool collision = CircleRect(circlePosition, radius, mouse, { rw * 0.5f, rh * 0.5f }, &mtv);
        Color color = collision ? RED : GREEN;
        if (collision)
            circlePosition = circlePosition + mtv;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(circlePosition, radius, color);
        DrawRectangleV({ mouse.x - rw * 0.5f , mouse.y - rh * 0.5f }, { rw, rh }, color);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
