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

// Visualize bezier polynomial expansion with Decasteljau's Algorithm (identical result)
void Visualize(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, float t)
{
    Vector2 A = Lerp(p0, p1, t);
    Vector2 B = Lerp(p1, p2, t);
    Vector2 C = Lerp(p2, p3, t);

    Vector2 D = Lerp(A, B, t);
    Vector2 E = Lerp(B, C, t);

    // Draw end (p0 & p3) and control (p1 & p2) points in grey
    DrawCircleV(p0, 10.0f, GRAY);
    DrawCircleV(p1, 10.0f, GRAY);
    DrawCircleV(p2, 10.0f, GRAY);
    DrawCircleV(p3, 10.0f, GRAY);

    // Draw lines between control & end points
    DrawLineEx(p0, p1, 5.0f, ORANGE);
    DrawLineEx(p1, p2, 5.0f, ORANGE);
    DrawLineEx(p2, p3, 5.0f, ORANGE);

    // Draw first layer of interpolation (3 inner points from 4 outer points)
    DrawLineEx(A, B, 5.0f, GOLD);
    DrawLineEx(B, C, 5.0f, GOLD);
    DrawCircleV(A, 10.0f, GOLD);
    DrawCircleV(B, 10.0f, GOLD);
    DrawCircleV(C, 10.0f, GOLD);

    // Draw second layer of interpolation (2 inner points from 3 outer points)
    DrawLineEx(D, E, 5.0f, GREEN);
    DrawCircleV(D, 10.0f, GREEN);
    DrawCircleV(E, 10.0f, GREEN);

    // Draw final point (1 resultant point given 2 initial points)
    DrawCircleV(Lerp(D, E, t), 20.0f, BLUE);

    // The following are identical:
    // Lerp(D, E, t) == Bezier(p0, p1, p2, p3, t);
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
        Visualize(p0, p1, p2, p3, t);
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
