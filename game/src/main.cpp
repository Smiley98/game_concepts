#include "rlImGui.h"
#include "Math.h"
#include <array>
#include <vector>
constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;
constexpr int TILE_COUNT = 20;
constexpr int TILE_WIDTH = SCREEN_WIDTH / TILE_COUNT;
constexpr int TILE_HEIGHT = SCREEN_HEIGHT / TILE_COUNT;
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

    Vector2 targetPosition{ SCREEN_WIDTH * 0.7f, SCREEN_HEIGHT * 0.25f };
    float targetRadius = 25.0f;

    vector<Circle> obstacles(5);
    for (Circle& obstacle : obstacles)
    {
        float maxRadius = 50.0f;
        Vector2 position = { Random(maxRadius, SCREEN_WIDTH - maxRadius), Random(maxRadius, SCREEN_HEIGHT - maxRadius) };
        float radius = Random(5.0f, maxRadius);
        obstacle = { position, radius };
    }

    float tileLength = 250.0f;
    array<array<Vector2, TILE_COUNT>, TILE_COUNT> tiles{};
    for (size_t row = 0; row < TILE_COUNT; row++)
    {
        for (size_t col = 0; col < TILE_COUNT; col++)
        {
            tiles[row][col] = { (float)col * TILE_WIDTH, (float)row * TILE_HEIGHT };
        }
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

        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (size_t row = 0; row < TILE_COUNT; row++)
        {
            for (size_t col = 0; col < TILE_COUNT; col++)
            {
                Vector2 tileStart{ tiles[row][col].x + TILE_WIDTH * 0.5f, tiles[row][col].y + TILE_HEIGHT * 0.5f };
                bool playerVisible = IsVisible(tileStart, tileLength, playerPosition, playerRadius, obstacles);
                bool targetVisible = IsVisible(tileStart, tileLength, targetPosition, targetRadius, obstacles);

                Color color = GRAY;
                if (playerVisible && targetVisible)
                    color = MAGENTA;
                else if (playerVisible)
                    color = DARKBLUE;
                else if (targetVisible)
                    color = DARKPURPLE;
                DrawRectangleV(tiles[row][col], { TILE_WIDTH, TILE_HEIGHT }, color);
            }
        }

        DrawLineV(playerPosition, playerPosition + playerDirection * playerLength, BLUE);
        DrawCircleV(playerPosition, playerRadius, BLUE);
        DrawCircleV(targetPosition, targetRadius, PURPLE);

        for (const Circle& obstacle : obstacles)
            DrawCircleV(obstacle.position, obstacle.radius, ORANGE);

        rlImGuiBegin();
        ImGui::SliderFloat("Tile View Distance", &tileLength, 100.0f, 500.0f);
        ImGui::SliderFloat2("Target Position", &targetPosition.x, 0.0f, SCREEN_WIDTH);
        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}