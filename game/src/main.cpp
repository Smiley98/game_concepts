#include "rlImGui.h"
#include "Math.h"
#include <array>
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

    array<Circle, 5> obstacles{};
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

        Vector2 targetEnd = targetPosition + Normalize(playerPosition - targetPosition) * viewDistance;
        bool playerVisible = LineCircle(targetPosition, targetEnd, playerPosition, playerRadius);

        // Only check obstacles if the player is within the target's view-distance (proximity radius)
        if (playerVisible)
        {
            vector<Vector2> intersections;
            for (const Circle& obstacle : obstacles)
            {
                // Store the obstacle's position if the obstacle is intersecting with the target's ray
                if (LineCircle(targetPosition, targetEnd, obstacle.position, obstacle.radius))
                    intersections.push_back(obstacle.position);
            }

            for (Vector2 poi : intersections)
            {
                if (Distance(targetPosition, playerPosition) > Distance(targetPosition, poi))
                {
                    playerVisible = false;
                    break;
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawCircleV(targetPosition, playerRadius, playerVisible ? GREEN : RED);
        DrawLineV(targetPosition, targetEnd, playerVisible ? GREEN : RED);

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