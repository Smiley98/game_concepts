#include "rlImGui.h"
#include "Math.h"
#include <algorithm>
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

bool LineCircle(Vector2 lineStart, Vector2 lineEnd, Vector2 circlePosition, float circleRadius)
{
    Vector2 nearest = ProjectPointLine(lineStart, lineEnd, circlePosition);
    return DistanceSqr(nearest, circlePosition) <= circleRadius * circleRadius;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    SetTargetFPS(60);

    Vector2 playerPosition{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    Vector2 playerDirection = Direction(0.0f);
    float playerSpeed = 250.0f;
    float playerRotationSpeed = 250.0f * DEG2RAD;
    float playerLength = 250.0f;

    Vector2 targetPosition{ SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.25f };
    float targetRadius = 25.0f;

    Vector2 obstaclePosition = Lerp(playerPosition, targetPosition, 0.5f);
    float obstacleRadius = 35.0f;

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

        Vector2 playerEnd = playerPosition + playerDirection * playerLength;
        bool targetCollision = LineCircle(playerPosition, playerEnd, targetPosition, targetRadius);
        bool obstacleCollision = LineCircle(playerPosition, playerEnd, obstaclePosition, obstacleRadius);

        // We're guaranteed to see the target if there's no obstacle collision,
        // So only do distance comparison if there's an obstacle collision (and target collision)
        // Calculating the exact points of intersection is expensive, so approximate distance using target & obstacle positions
        bool targetVisible = targetCollision;
        if (obstacleCollision && targetCollision)
        {
            float targetDistance = Distance(playerPosition, targetPosition);
            float obstacleDistance = Distance(playerPosition, obstaclePosition);
            targetVisible = targetDistance < obstacleDistance;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawLineV(playerPosition, playerEnd, BLUE);
        DrawCircleV(playerPosition, 25.0f, BLUE);
        DrawCircleV(targetPosition, targetRadius, targetVisible ? RED : GREEN);
        DrawCircleV(obstaclePosition, obstacleRadius, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
