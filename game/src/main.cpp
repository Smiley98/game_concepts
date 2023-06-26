#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// "Decasteljau's Algorithm" -- Move along a curve by interpolating between nested lines
Vector2 Decasteljau(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, float t)
{
    Vector2 A = Lerp(p0, p1, t);
    Vector2 B = Lerp(p1, p2, t);
    Vector2 C = Lerp(p2, p3, t);

    Vector2 D = Lerp(A, B, t);
    Vector2 E = Lerp(B, C, t);

    return Lerp(D, E, t);
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
        DrawCircleV(Decasteljau(p0, p1, p2, p3, t), 20.0f, RED);
        DrawText("Drag the sliders to see how each point affects the curve", 10, 10, 20, GRAY);

        rlImGuiBegin();
        ImGui::SliderFloat2("End Point 1", &p0.x, 0.0f, SCREEN_WIDTH);
        ImGui::SliderFloat2("End Point 2", &p3.x, 0.0f, SCREEN_WIDTH);
        ImGui::SliderFloat2("Control Point 1", &p1.x, 0.0f, SCREEN_WIDTH);
        ImGui::SliderFloat2("Control Point 2", &p2.x, 0.0f, SCREEN_WIDTH);
        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
