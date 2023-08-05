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

// True if distance between point and circle-center is less than the circle's radius
//bool PointCircle(Vector2 point, Vector2 circlePosition, float circleRadius)
//{
//    return DistanceSqr(point, circlePosition) <= circleRadius * circleRadius;
//}

// True of the distance between the circle and the circle's projection onto the line is less than the circle's radius
bool LineCircle(Vector2 lineStart, Vector2 lineEnd, Vector2 circlePosition, float circleRadius)
{
    Vector2 nearest = ProjectPointLine(lineStart, lineEnd, circlePosition);
    return DistanceSqr(nearest, circlePosition) <= circleRadius * circleRadius;
}

// Do line-circle test for all lines, then do a point-polygon test to check for containment
bool CirclePolygon(Vector2 circlePosition, float circleRadius, const std::vector<Vector2>& polygon)
{
    for (size_t i = 0; i < polygon.size() - 1; i++)
    {
        Vector2 vc = polygon[i];        // "current vertex"
        Vector2 vn = polygon[i + 1];    // "next vertex"
        if (LineCircle(vc, vn, circlePosition, circleRadius))
            return true;
    }
    return CheckCollisionPointPoly(circlePosition, const_cast<std::vector<Vector2>&>(polygon).data(), polygon.size());
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    float margin = 0.35f;
    std::vector<Vector2> points = Hexagon(margin);
    float radius = 25.0f;

    while (!WindowShouldClose())
    {
        bool collision = CirclePolygon(GetMousePosition(), radius, points);
        Color color = collision ? RED : GREEN;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(GetMousePosition(), radius, color);
        DrawLineStrip(points.data(), points.size(), color);
        rlImGuiBegin();
        if (ImGui::SliderFloat("Polygon Margin", &margin, 0.0f, 1.0f))
        {
            points = Hexagon(margin);
        }
        ImGui::SliderFloat("Circle Radius", &radius, 1.0f, 50.0f);
        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
