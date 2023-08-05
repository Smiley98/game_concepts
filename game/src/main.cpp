#include "rlImGui.h"
#include "Math.h"
#include <vector>
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Returns the points of a hexagon based on margin between screen edges as a percentage (inverts after 50%)
std::vector<Vector2> Hexagon(float margin)
{
    float tMin = Clamp(margin, 0.0f, 1.0f);
    float tMax = 1.0f - tMin;
    float xMin = SCREEN_WIDTH * tMin;
    float xMax = SCREEN_WIDTH * tMax;
    float yMin = SCREEN_HEIGHT * tMin;
    float yMax = SCREEN_HEIGHT * tMax;
    float xMid = (xMin + xMax) * 0.5f;
    float yTop = yMin - (yMax - yMin) * 0.5f;
    float yBot = yMax + (yMax - yMin) * 0.5f;
    return {
        { xMin, yMin },  // top left
        { xMid, yTop },  // top middle
        { xMax, yMin },  // top right
        { xMax, yMax },  // bottom right
        { xMid, yBot },  // bottom middle
        { xMin, yMax },  // bottom left
        { xMin, yMin }   // connect end to start
        //(needed for line-strip rendering and point-polygon collision testing)
    };
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    float margin = 0.35f;
    std::vector<Vector2> points = Hexagon(margin);

    while (!WindowShouldClose())
    {
        // Tunnel into this function to see the math, and click the web-link to visit an excellent website!
        bool collision = CheckCollisionPointPoly(GetMousePosition(), points.data(), points.size());
        Color color = collision ? RED : GREEN;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(GetMousePosition(), 5.0f, DARKGRAY);
        DrawLineStrip(points.data(), points.size(), color);
        rlImGuiBegin();
        if (ImGui::SliderFloat("Polygon Margin", &margin, 0.0f, 1.0f))
        {
            points = Hexagon(margin);
        }
        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
