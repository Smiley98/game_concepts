#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

bool InFoV(Vector2 viewerPosition, Vector2 viewerDirection, float viewDistance, float FoV/*radians*/, Vector2 targetPosition)
{
    // Don't bother with angle-check if target is far away from viewer
    if (Distance(viewerPosition, targetPosition) > viewDistance) return false;
    // Optimization: compare squared distances to avoid expensive square-root calculations
    //if (DistanceSqr(viewerPosition, targetPosition) > viewDistance * viewDistance) return false;

    // Vector from A to B = B - A
    Vector2 targetDirection = Normalize(targetPosition - viewerPosition);

    // Determine if the angle between viewer and target (dot product) is less than
    // half the viewer's field of view (half to form a right-triangle)
    float angle = acosf(Dot(viewerDirection, targetDirection));
    return angle <= FoV * 0.5f;

    // Optimization: Since arccos is the inverse of cos, we can flip the comparison to avoid an inverse trig function
    //return Dot(viewerDirection, targetDirection) > cosf(FoV * 0.5f);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);
    rlImGuiSetup(true);

    float FoV = 60.0f * DEG2RAD;  // field of view
    float viewDistance = 500.0f;
    Vector2 viewerPosition{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    Vector2 targetPosition{ SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.5f };

    while (!WindowShouldClose())
    {
        Vector2 viewerDirection = Direction(GetTime());
        bool targetVisible = InFoV(viewerPosition, viewerDirection, viewDistance, FoV, targetPosition);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(viewerPosition, 25.0f, BLUE);
        DrawLineV(viewerPosition, viewerPosition + Rotate(viewerDirection, FoV *  0.5f) * viewDistance, BLUE);
        DrawLineV(viewerPosition, viewerPosition + Rotate(viewerDirection, FoV * -0.5f) * viewDistance, BLUE);
        DrawCircleV(targetPosition, 25.0f, targetVisible ? GREEN : RED);
        rlImGuiBegin();
        ImGui::SliderAngle("Field of View", &FoV, 10.0f, 100.0f);
        ImGui::SliderFloat("View Distance", &viewDistance, 100.0f, 750.0f);
        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
