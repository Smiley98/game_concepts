#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

bool CircleCircle(Vector2 position1, Vector2 position2, float radius1, float radius2)
{
    return Distance(position1, position2) <= radius1 + radius2;
}

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
}

int main(void)
{
    InitWindow(1280, 720, "Game");
    SetTargetFPS(60);

    const Vector2 center{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    const float radius = 50.0f;

    while (!WindowShouldClose())
    {
        const Vector2 mouse = GetMousePosition();
        const float rw = 60.0f;
        const float rh = 40.0f;

        Color color = CircleRect(center, radius, mouse, { rw * 0.5f, rh * 0.5f }) ? RED : GREEN;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawCircleV(center, radius, color);

        Rectangle r;
        r.x = mouse.x - rw * 0.5f;
        r.y = mouse.y - rh * 0.5f;
        r.width = rw;
        r.height = rh;
        DrawRectangleRec(r, color);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

// Optimization -- square distance and radii to remove square-root from calculation
//bool CircleCircle(Vector2 position1, Vector2 position2, float radius1, float radius2)
//{
//    return DistanceSqr(position1, position2) <= powf((radius1 + radius2), 2.0f);
//}