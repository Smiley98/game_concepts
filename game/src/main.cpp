#include "rlImGui.h"
#include "Math.h"
#include <cstdio>
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
//
// Finally, we determine which points of intersection actually lie on the line
// by checking if their projections are within the normalized range (0 >= t <= 1)
struct Hit
{
    Vector2 poi{};      // point of intersection
    bool hit = false;   // whether poi is valid
};

// Geometrical solution (solve the triangle as mentioned above)
bool LineCircle1(Vector2 lineStart, Vector2 lineEnd, Vector2 circlePosition, float circleRadius, Hit& hit1, Hit& hit2)
{
    Vector2 proj = ProjectPointLine(lineStart, lineEnd, circlePosition);
    float c2 = circleRadius * circleRadius;
    bool collision = DistanceSqr(proj, circlePosition) < c2;
    if (collision)
    {
        float a2 = DistanceSqr(proj, circlePosition);
        float b = sqrtf(c2 - a2);

        Vector2 line = lineEnd - lineStart;
        Vector2 lineDirection = Normalize(line);
        float lineLength2 = LengthSqr(line);
        
        Vector2 poi1 = proj - lineDirection * b;
        Vector2 poi2 = proj + lineDirection * b;
        float t1 = Dot(poi1 - lineStart, line) / lineLength2;
        float t2 = Dot(poi2 - lineStart, line) / lineLength2;

        hit1.poi = poi1;
        hit2.poi = poi2;
        hit1.hit = t1 >= 0.0f && t1 <= 1.0f;
        hit2.hit = t2 >= 0.0f && t2 <= 1.0f;
    }
    return collision;
}

// Analytical solution (solves a quadratic -- similar to the geometric solution, but with calculus) 
bool LineCircle2(Vector2 lineStart, Vector2 lineEnd, Vector2 circlePosition, float circleRadius, Hit& hit1, Hit& hit2)
{
    Vector2 dc = lineStart - circlePosition;
    Vector2 dx = lineEnd - lineStart;
    float a = LengthSqr(dx);
    float b = Dot(dx, dc) * 2.0f;
    float c = LengthSqr(dc) - circleRadius * circleRadius;
    float det = b * b - 4.0f * a * c;

    if (a <= FLT_EPSILON || det < 0.0f) return false;

    det = sqrtf(det);
    float t1 = (-b - det) / (2.0f * a);
    float t2 = (-b + det) / (2.0f * a);

    hit1.poi = lineStart + dx * t1;
    hit2.poi = lineStart + dx * t2;
    hit1.hit = t1 >= 0.0f && t1 <= 1.0f;
    hit2.hit = t2 >= 0.0f && t2 <= 1.0f;

    return true;
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

        Hit hit1, hit2;
        bool collision = LineCircle2(lineStart, lineEnd, circlePosition, radius, hit1, hit2);
        Color color = collision ? RED : GREEN;
        //bool collision = LineCircle1(lineStart, lineEnd, circlePosition, radius, hit1, hit2);
        // This gets a little glitchy around the edges, so the analytical solution is prefered.

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawLineEx(lineStart, lineEnd, 1.0f, color);
        DrawCircleV(circlePosition, radius, color);
        if (collision)
        {
            if (hit1.hit)
                DrawCircleV(hit1.poi, 5.0f, ORANGE);
            if (hit2.hit)
                DrawCircleV(hit2.poi, 5.0f, ORANGE);
        }
        DrawText("Move the circle with your mouse.", 10, 10, 20, GRAY);
        DrawText("Hold E & Q to rotate the line clockwise/counter-clockwise.", 10, 30, 20, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
