#include "rlImGui.h"
#include "Math.h"
#include <vector>
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

// Determine control & end points along spline given an index
void Points(const std::vector<Vector2>& points, int index,
    Vector2& p0, Vector2& p1, Vector2& p2, Vector2& p3)
{
    const int n = points.size();
    p0 = points[(index - 1 + n) % n];
    p1 = points[index % n];
    p2 = points[(index + 1) % n];
    p3 = points[(index + 2) % n];
}

// Convenience overload for ImGui
void Points(std::vector<Vector2>& points, int index,
    Vector2*& p0, Vector2*& p1, Vector2*& p2, Vector2*& p3)
{
    const int n = points.size();
    p0 = &points[(index - 1 + n) % n];
    p1 = &points[index % n];
    p2 = &points[(index + 1) % n];
    p3 = &points[(index + 2) % n];
}

int main(void)
{
    InitWindow(1280, 720, "Game");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    int index = 0;
    std::vector<Vector2> points
    {
        { SCREEN_WIDTH * 0.2f, SCREEN_HEIGHT * 0.75f },
        { SCREEN_WIDTH * 0.4f, SCREEN_HEIGHT * 0.25f },
        { SCREEN_WIDTH * 0.6f, SCREEN_HEIGHT * 0.25f },
        { SCREEN_WIDTH * 0.8f, SCREEN_HEIGHT * 0.75f },
    };

    float elapsed = 0.0f;
    const float duration = 1.0f;

    bool drawCurve = true;
    bool paused = false;
    while (!WindowShouldClose())
    {
        const float t = elapsed / duration;
        if (!paused)
        {
            // Advance indices if time duration is exceeded
            if (elapsed >= duration)
            {
                elapsed = 0.0f;
                ++index %= points.size();
            }
            elapsed += GetFrameTime();
        }

        Vector2 p0, p1, p2, p3;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Render entire spline
        if (drawCurve)
        {
            for (size_t i = 0; i < points.size(); i++)
            {
                Points(points, i, p0, p1, p2, p3);
                DrawLineCatmull(p0, p1, p2, p3, { 130, 130, 130, 64 }, 5.0f);
                DrawCircleV(points[i], 10.0f, DARKGRAY);
            }
        }

        // Render previous interval
        Points(points, (index - 1 + points.size()) % points.size(), p0, p1, p2, p3);
        DrawLineCatmull(p0, p1, p2, p3, GREEN, 5.0f, true);
        DrawCircleV(p1, 20.0f, GREEN);

        // Render current interval
        Points(points, index, p0, p1, p2, p3);
        DrawLineCatmull(p0, p1, p2, p3, BLUE, 5.0f);
        DrawCircleV(p1, 20.0f, BLUE);
        DrawCircleV(p2, 20.0f, BLUE);
        DrawCircleV(Catmull(p0, p1, p2, p3, t), 20.0f, BLUE);

        // Render next interval
        Points(points, (index + 1) % points.size(), p0, p1, p2, p3);
        DrawLineCatmull(p0, p1, p2, p3, PURPLE, 5.0f, true);
        DrawCircleV(p2, 20.0f, PURPLE);

        DrawText("Drag the sliders to see how each point affects the curve", 10, 10, 20, GRAY);
        DrawText("Green = previous interval", 10, 30, 20, GREEN);
        DrawText("Blue = current interval", 10, 50, 20, BLUE);
        DrawText("Purple = next interval", 10, 70, 20, PURPLE);
        DrawText("(Control points are previous & next intervals)", 10, 90, 20, GRAY);
        DrawText("(End points are along the current interval)", 10, 110, 20, GRAY);

        rlImGuiBegin();
        Vector2* cp0, * ep0, * ep1, * cp1;
        Points(points, index, cp0, ep0, ep1, cp1);
        ImGui::SliderFloat2("Control Point 1", (float*)cp0, 0.0f, SCREEN_WIDTH);
        ImGui::SliderFloat2("End Point 1", (float*)ep0, 0.0f, SCREEN_WIDTH);
        ImGui::SliderFloat2("End Point 2", (float*)ep1, 0.0f, SCREEN_WIDTH);
        ImGui::SliderFloat2("Control Point 2", (float*)cp1, 0.0f, SCREEN_WIDTH);
        ImGui::Checkbox("Paused? (Check before editing points)", &paused);
        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
