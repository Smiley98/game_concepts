#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

bool CircleCircle(Vector2 position1, Vector2 position2, float radius1, float radius2)
{
    return Distance(position1, position2) <= radius1 + radius2;
}

int main(void)
{
    InitWindow(1280, 720, "Game");
    SetTargetFPS(60);

    const Vector2 center{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    const float radius = 50.0f;

    while (!WindowShouldClose())
    {
        const Vector2 cursor = GetMousePosition();

        const bool collision = CircleCircle(cursor, center, radius, radius);
        const Color color = collision ? RED : GREEN;

        const float radiiSum = radius + radius;
        const float distance = Distance(cursor, center);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawCircleV(center, radius, color);
        DrawCircleV(cursor, radius, color);

        const float thickness = 5.0f;
        Vector2 start{ SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.75f };
        DrawLineEx(start, start + Vector2{ 1.0f, 0.0f } * radiiSum, thickness, BLUE);
        start.y += thickness;
        DrawLineEx(start, start + Vector2{ 1.0f, 0.0f } * distance, thickness, PURPLE);

        DrawText(TextFormat("Radii Sum: %f", radiiSum), 10, 10, 20, BLUE);
        DrawText(TextFormat("Distance: %f", distance), 10, 30, 20, PURPLE);

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