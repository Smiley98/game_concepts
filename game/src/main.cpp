#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// mtv resolves 1 from 2
bool CircleCircle(Vector2 position1, Vector2 position2, float radius1, float radius2, Vector2* mtv = nullptr)
{
    float distance = Distance(position1, position2);
    float radiiSum = radius1 + radius2;
    bool collision = distance <= radiiSum;
    if (collision && mtv != nullptr)
    {
        *mtv = Normalize(position1 - position2) * (radiiSum - distance);
    }
    return collision;
}

int main(void)
{
    InitWindow(1280, 720, "Game");
    SetTargetFPS(60);

    const Vector2 center{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    const float radius = 50.0f;
    Vector2 position = center;
    while (!WindowShouldClose())
    {
        const Vector2 cursor = GetMousePosition();

        Vector2 mtv;
        const bool collision = CircleCircle(position, cursor, radius, radius, &mtv);
        if (collision)
            position = position + mtv;
        const Color color = CircleCircle(position, cursor, radius, radius) ? RED : GREEN;

        const float radiiSum = radius + radius;
        const float distance = Distance(position, cursor);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawCircleV(position, radius, color);
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