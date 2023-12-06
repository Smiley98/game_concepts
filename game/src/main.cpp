#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

bool CircleRect(Vector2 circleCenter, float radius, Vector2 rectCenter, Vector2 halfExtents)
{
    Vector2 nearest;
    float xMin = rectCenter.x - halfExtents.x;
    float xMax = rectCenter.x + halfExtents.x;
    float yMin = rectCenter.y - halfExtents.y;
    float yMax = rectCenter.y + halfExtents.y;
    nearest.x = fabsf(circleCenter.x - xMin) < fabsf(circleCenter.x - xMax) ? xMin : xMax;
    nearest.y = fabsf(circleCenter.y - yMin) < fabsf(circleCenter.y - yMax) ? yMin : yMax;
    return Distance(circleCenter, nearest) <= radius;
    // Optimization -- return DistanceSqr(circleCenter, nearest) <= radius * radius;
}

int main(void)
{
    InitWindow(1280, 720, "Game");
    SetTargetFPS(60);

    const Vector2 center{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    const float radius = 50.0f;

    while (!WindowShouldClose())
    {
        const float rw = 60.0f;
        const float rh = 40.0f;
        const Vector2 mouse = GetMousePosition();
        const Vector2 circleCenter = center;
        const Vector2 rectCenter = mouse;
        Color color = CircleRect(center, radius, mouse, { rw * 0.5f, rh * 0.5f }) ? RED : GREEN;


        Vector2 nearest;
        float xMin = rectCenter.x - rw * 0.5f;
        float xMax = rectCenter.x + rw * 0.5f;
        float yMin = rectCenter.y - rh * 0.5f;
        float yMax = rectCenter.y + rh * 0.5f;

        float dxMin = fabsf(circleCenter.x - xMin);
        float dxMax = fabsf(circleCenter.x - xMax);
        float dyMin = fabsf(circleCenter.y - yMin);
        float dyMax = fabsf(circleCenter.y - yMax);

        nearest.x = fabsf(circleCenter.x - xMin) < fabsf(circleCenter.x - xMax) ? xMin : xMax;
        nearest.y = fabsf(circleCenter.y - yMin) < fabsf(circleCenter.y - yMax) ? yMin : yMax;

        Vector2 start{ 100.0f, 100.0f };
        DrawLineEx(start, { start.x + dxMin, start.y }, 5.0f, RED);
        start.y += 5.0f;
        DrawLineEx(start, { start.x + dxMax, start.y }, 5.0f, ORANGE);
        start.y += 5.0f;
        DrawLineEx(start, { start.x + dyMin, start.y }, 5.0f, GREEN);
        start.y += 5.0f;
        DrawLineEx(start, { start.x + dyMax, start.y }, 5.0f, BLUE);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(center, radius, color);
        DrawRectangleV({ mouse.x - rw * 0.5f, mouse.y - rh * 0.5f }, { rw, rh }, color);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
