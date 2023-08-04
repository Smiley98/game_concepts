#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// We can solve a right-triangle to determine the points of intersection between the line and circle:
// let A = projection of line onto circle
// let C = circle center
// let a = Length(C - A)
// let c = circle radius
// let B = point of intersection ("poi")
// let b = Length(B - A)
// let D = direction of line
// 
// b and B are unknown.
// Solve for b using Pythagorean theorem
// b = sqrt(c^2 - a^2)
// 
// Solve for B by translating from projection by b units in the direction of the line:
// B = A + D * b
bool LineCircle(Vector2 lineStart, Vector2 lineEnd, Vector2 circlePosition, float circleRadius, Vector2* poi1, Vector2* poi2)
{
    Vector2 proj = ProjectPointLine(lineStart, lineEnd, circlePosition);
    float c2 = circleRadius * circleRadius;
    bool collision = DistanceSqr(proj, circlePosition) < c2;
    if (collision)
    {
        float a2 = DistanceSqr(proj, circlePosition);
        float b = sqrtf(c2 - a2);
        Vector2 direction = Normalize(lineEnd - lineStart);
        *poi1 = proj - direction * b;
        *poi2 = proj + direction * b;
        // TODO -- use t-value of projection to determine whether pois are outside of line
    }
    return collision;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);

    Vector2 center{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    Vector2 direction{ 1.0f, 0.0f };
    float length = 500.0f;
    float thickness = 5.0f;
    float radius = 20.0f;

    HideCursor();
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

        Vector2 poi1{}, poi2{};
        bool collision = LineCircle(lineStart, lineEnd, circlePosition, radius, &poi1, &poi2);
        Color color = collision ? RED : GREEN;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawLineEx(lineStart, lineEnd, thickness, color);
        DrawCircleV(circlePosition, radius, color);
        if (collision)
        {
            DrawCircleV(poi1, 5.0f, ORANGE);
            DrawCircleV(poi2, 5.0f, ORANGE);
        }
        DrawText("Move the circle with your mouse.", 10, 10, 20, GRAY);
        DrawText("Hold E & Q to rotate the line clockwise/counter-clockwise.", 10, 30, 20, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
