#include "rlImGui.h"
#include "Math.h"
#include <vector>
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
using namespace std;

bool LineCircle(Vector2 lineStart, Vector2 lineEnd, Vector2 circlePosition, float circleRadius)
{
    Vector2 nearest = ProjectPointLine(lineStart, lineEnd, circlePosition);
    return DistanceSqr(nearest, circlePosition) <= circleRadius * circleRadius;
}

struct Circle
{
    Vector2 position;
    float radius;
};

bool IsVisible(Vector2 viewer, float viewDistance, Vector2 target, float targetRadius, const vector<Circle>& obstacles)
{
    Vector2 viewerEnd = viewer + Normalize(target - viewer) * viewDistance;
    if (LineCircle(viewer, viewerEnd, target, targetRadius))
    {
        vector<Vector2> intersections;
        for (const Circle& obstacle : obstacles)
        {
            if (LineCircle(viewer, viewerEnd, obstacle.position, obstacle.radius))
                intersections.push_back(obstacle.position);
        }

        // Optimization: compare squared distances instead to avoid (expensive) square-root calculations!
        float targetDistance = DistanceSqr(viewer, target);
        for (Vector2 poi : intersections)
        {
            if (DistanceSqr(viewer, poi) < targetDistance)
                return false;
        }
        return true;
    }
    return false;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    Vector2 playerPosition{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    Vector2 playerDirection = Direction(0.0f);
    float playerSpeed = 250.0f;
    float playerRotationSpeed = 250.0f * DEG2RAD;
    float playerLength = 250.0f;
    float playerRadius = 20.0f;
    float viewDistance = 1000.0f;

    Vector2 targetPosition{ SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.75f };
    float targetRadius = playerRadius;

    vector<Circle> obstacles(5);
    for (Circle& obstacle : obstacles)
    {
        float maxRadius = 50.0f;
        obstacle.position = { Random(maxRadius, SCREEN_WIDTH - maxRadius), Random(maxRadius, SCREEN_HEIGHT - maxRadius) };
        obstacle.radius = Random(5.0f, maxRadius);
    }

    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();
        const float playerSpeedDelta = playerSpeed * dt;
        const float playerRotationDelta = playerRotationSpeed * dt;

        if (IsKeyDown(KEY_W))
        {
            playerPosition = playerPosition + playerDirection * playerSpeedDelta;
        }
        if (IsKeyDown(KEY_S))
        {
            playerPosition = playerPosition - playerDirection * playerSpeedDelta;
        }
        if (IsKeyDown(KEY_D))
        {
            playerDirection = Rotate(playerDirection, playerRotationDelta);
        }
        if (IsKeyDown(KEY_A))
        {
            playerDirection = Rotate(playerDirection, -playerRotationDelta);
        }
        bool playerVisible = IsVisible(playerPosition, viewDistance, targetPosition, targetRadius, obstacles);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawCircleV(targetPosition, playerRadius, playerVisible ? GREEN : RED);
        DrawLineV(targetPosition, targetPosition + Normalize(playerPosition - targetPosition) * viewDistance, playerVisible ? GREEN : RED);

        DrawCircleV(playerPosition, playerRadius, BLUE);
        DrawLineV(playerPosition, playerPosition + playerDirection * playerLength, DARKBLUE);

        for (const Circle& obstacle : obstacles)
            DrawCircleV(obstacle.position, obstacle.radius, GRAY);

        rlImGuiBegin();
        ImGui::SliderFloat("View Distance", &viewDistance, 10.0f, 1250.0f);
        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}