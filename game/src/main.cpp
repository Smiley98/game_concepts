#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Catmull polynomial expansion
Vector2 Catmull(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, float t)
{
    // f(t) = 0.5(t^3, t^2, t, 1)MP
    // M =
    // [ -1,  3, -3,  1]
    // [  2, -5,  4, -1]
    // [ -1,  0,  1,  0]
    // [  0,  2,  0,  0]

    // MP = constraint matrix * points
    Vector2 r0 = p0 * -1.0f + p1 * 3.0f + p2 * -3.0f + p3;
    Vector2 r1 = p0 * 2.0f + p1 * -5.0f + p2 * 4.0f - p3;
    Vector2 r2 = p0 * -1.0f + p2;
    Vector2 r3 = p1 * 2.0f;

    // U = au^3 + bu^2 + cu + d (cubic polynomial * t)
    Vector2 p = (r3 + (r2 * t) + (r1 * t * t) + (r0 * t * t * t)) * 0.5f;
    return p;
}

// Render the curve as a series of lines along a catmull spline
void DrawLineCatmull(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3,
    Color color, float thick = 1.0f, bool dotted = false, size_t lines = 16)
{
    float t = 0.0f;
    const float step = 1.0f / (float)lines;
    for (size_t i = 0; i < lines; i++)
    {
        Vector2 start = Catmull(p0, p1, p2, p3, t);
        Vector2 end = Catmull(p0, p1, p2, p3, t + step);
        t += step;
        if (dotted && i % 2 == 0) continue;
        DrawLineEx(start, end, thick, color);
    }
}

int main(void)
{
    InitWindow(1280, 720, "Game");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    Vector2 p0{ SCREEN_WIDTH * 0.2f, SCREEN_HEIGHT * 0.75f };   // Control Point 1
    Vector2 p1{ SCREEN_WIDTH * 0.4f, SCREEN_HEIGHT * 0.25f };   // End Point 1
    Vector2 p2{ SCREEN_WIDTH * 0.6f, SCREEN_HEIGHT * 0.25f };   // End Point 2
    Vector2 p3{ SCREEN_WIDTH * 0.8f, SCREEN_HEIGHT * 0.75f };   // Control Point 2

    while (!WindowShouldClose())
    {
        // Repeat between 0 to 1 and 1 to 0 to move back and forth along the curve
        // (Enter "cos(x) * 0.5 + 0.5" into Desmos Graphing Calculator)
        const float t = cosf(GetTime()) * 0.5f + 0.5f;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawLineCatmull(p0, p1, p2, p3, RED, 10.0f);
        DrawCircleV(p0, 10.0f, GRAY);
        DrawCircleV(p1, 10.0f, GRAY);
        DrawCircleV(p2, 10.0f, GRAY);
        DrawCircleV(p3, 10.0f, GRAY);
        DrawCircleV(Catmull(p0, p1, p2, p3, t), 20.0f, BLUE);
        DrawText("Drag the sliders to see how each point affects the curve", 10, 10, 20, GRAY);

        rlImGuiBegin();
        ImGui::SliderFloat2("Control Point 1", &p0.x, 0.0f, SCREEN_WIDTH);
        ImGui::SliderFloat2("End Point 1", &p1.x, 0.0f, SCREEN_WIDTH);
        ImGui::SliderFloat2("End Point 2", &p2.x, 0.0f, SCREEN_WIDTH);
        ImGui::SliderFloat2("Control Point 2", &p3.x, 0.0f, SCREEN_WIDTH);
        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
