#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Bezier polynomial expansion -- An alternative to Decasteljau's Algorithm 
Vector2 Bezier(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, float t)
{
    // f(t) = (t^3, t^2, t, 1)MP
    // M =
    // [ -1,  3, -3,  1]
    // [  3, -6,  3,  0]
    // [ -3,  3,  0,  0]
    // [  1,  0,  0,  0]

    // MP = constraint matrix * points
    Vector2 r0 = p0 * -1.0f + p1 * 3.0f + p2 * -3.0f + p3;
    Vector2 r1 = p0 * 3.0f + p1 * -6.0f + p2 * 3.0f;
    Vector2 r2 = p0 * -3.0f + p1 * 3.0f;
    Vector2 r3 = p0;

    // U = au^3 + bu^2 + cu + d (cubic polynomial * t)
    Vector2 p = (r0 * t * t * t) + (r1 * t * t) + (r2 * t) + r3;
    return p;
}

int main(void)
{
    InitWindow(1280, 720, "Game");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    Vector2 p0{ SCREEN_WIDTH * 0.2f, SCREEN_HEIGHT * 0.75f };   // End Point 1
    Vector2 p1{ SCREEN_WIDTH * 0.4f, SCREEN_HEIGHT * 0.25f };   // Control Point 1
    Vector2 p2{ SCREEN_WIDTH * 0.6f, SCREEN_HEIGHT * 0.25f };   // Control Point 2
    Vector2 p3{ SCREEN_WIDTH * 0.8f, SCREEN_HEIGHT * 0.75f };   // End Point 2

    while (!WindowShouldClose())
    {
        // Repeat between 0 to 1 and 1 to 0 to move back and forth along the curve
        // (Enter "cos(x) * 0.5 + 0.5" into Desmos Graphing Calculator)
        const float t = cosf(GetTime()) * 0.5f + 0.5f;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawLineBezierCubic(p0, p3, p1, p2, 10.0f, RED);
        DrawCircleV(p0, 10.0f, GRAY);
        DrawCircleV(p1, 10.0f, GRAY);
        DrawCircleV(p2, 10.0f, GRAY);
        DrawCircleV(p3, 10.0f, GRAY);
        DrawCircleV(Bezier(p0, p1, p2, p3, t), 20.0f, GRAY);
        DrawText("Drag the sliders to see how each point affects the curve", 10, 10, 20, GRAY);

        rlImGuiBegin();
        ImGui::SliderFloat2("End Point 1", &p0.x, 0.0f, SCREEN_WIDTH);
        ImGui::SliderFloat2("Control Point 1", &p1.x, 0.0f, SCREEN_WIDTH);
        ImGui::SliderFloat2("Control Point 2", &p2.x, 0.0f, SCREEN_WIDTH);
        ImGui::SliderFloat2("End Point 2", &p3.x, 0.0f, SCREEN_WIDTH);
        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
