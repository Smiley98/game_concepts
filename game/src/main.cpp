#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Project circle-position onto line segment, then perform a point-circle collision check with the projected point!
bool LineCircle(Vector2 lineStart, Vector2 lineEnd, Vector2 circlePosition, float circleRadius)
{
    Vector2 nearest = ProjectPointLine(lineStart, lineEnd, circlePosition);
    return DistanceSqr(nearest, circlePosition) <= circleRadius * circleRadius;
    // More optimal to compare squared distance instead of using a more expensive square-root calculation!
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);

    Vector2 center{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    Vector2 direction{ 1.0f, 0.0f };
    float length = 500.0f;
    float thickness = 5.0f;

    while (!WindowShouldClose())
    {
        Vector2 circlePosition = GetMousePosition();

        float rotation = 250.0f * DEG2RAD * GetFrameTime();
        if (IsKeyDown(KEY_E))
            direction = Rotate(direction, rotation);
        if (IsKeyDown(KEY_Q))
            direction = Rotate(direction, -rotation);

        Vector2 lineStart = center + direction * length * 0.5f;
        Vector2 lineEnd = center + direction * length * -0.5f;

        // Project circle-position onto the line segment
        Vector2 projection = ProjectPointLine(lineStart, lineEnd, circlePosition);

        bool collision = LineCircle(lineStart, lineEnd, circlePosition, 20.0f);
        Color color = collision ? RED : GREEN;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(circlePosition, 20.0f, color);
        DrawCircleV(projection, 10.0f, GRAY);
        DrawLineEx(lineStart, lineEnd, thickness, color);
        DrawText("Move the circle with your mouse.", 10, 10, 20, GRAY);
        DrawText("Hold E & Q to rotate the line clockwise/counter-clockwise.", 10, 30, 20, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
