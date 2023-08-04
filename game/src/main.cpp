#include "rlImGui.h"
#include "Math.h"
#include <cstdio>
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// MTV (minimum translation vector) resolves circle from line (only written to on-collision)
bool LineCircle(Vector2 lineStart, Vector2 lineEnd, Vector2 circlePosition, float circleRadius, Vector2* mtv = nullptr)
{
    Vector2 nearest = ProjectPointLine(lineStart, lineEnd, circlePosition);
    bool collision = DistanceSqr(nearest, circlePosition) <= circleRadius * circleRadius;
    if (collision && mtv != nullptr)
    {
        // Compare distance from projection to center vs radius to determine penetration depth,
        // then translate by penetration depth in the direction from circle to projection!
        Vector2 resolutionDirection = Normalize(circlePosition - nearest);
        float resolutionLength = circleRadius - Distance(nearest, circlePosition);
        *mtv = resolutionDirection * resolutionLength;
    }
    return collision;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);

    Vector2 lineCenter{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    Vector2 lineDirection{ 1.0f, 0.0f };
    float lineLength = 500.0f;

    HideCursor();
    while (!WindowShouldClose())
    {
        Vector2 circlePosition = GetMousePosition();
        float circleRadius = 20.0f;

        float rotation = 250.0f * DEG2RAD * GetFrameTime();
        if (IsKeyDown(KEY_E))
            lineDirection = Rotate(lineDirection, rotation);
        if (IsKeyDown(KEY_Q))
            lineDirection = Rotate(lineDirection, -rotation);

        Vector2 lineStart = lineCenter + lineDirection * lineLength * 0.5f;
        Vector2 lineEnd = lineCenter + lineDirection * lineLength * -0.5f;
        Vector2 mtv;

        // More useful to resolve line from circle than circle from line
        // (since the circle will just flip from one side to the other of the line).
        //if (LineCircle(lineStart, lineEnd, circlePosition, circleRadius, &mtv))
        //    circlePosition = circlePosition + mtv;
        if (LineCircle(lineStart, lineEnd, circlePosition, circleRadius, &mtv))
            lineCenter = lineCenter - mtv;
        
        // Determine colour after collision-resolution
        Color color = LineCircle(lineStart, lineEnd, circlePosition, circleRadius) ? RED : GREEN;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawLineEx(lineStart, lineEnd, 5.0f, color);
        DrawCircleV(circlePosition, circleRadius, color);
        DrawCircleV(ProjectPointLine(lineStart, lineEnd, circlePosition), 5.0f, GRAY);
        DrawText("Move the circle with your mouse.", 10, 10, 20, GRAY);
        DrawText("Hold E & Q to rotate the line clockwise/counter-clockwise.", 10, 30, 20, GRAY);
        DrawText("Circle-projection in grey.", 10, 50, 20, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
