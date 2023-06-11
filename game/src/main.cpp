#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    Vector2 position{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    Vector2 direction = Direction(0.0f);
    float linearSpeed = 250.0f;     // 250 pixels per second
    float angularSpeed = 100.0f;    // 100 degrees per second

    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();
        const float frameLinearSpeed = linearSpeed * dt;            // Pixels per frame
        const float frameAngularSpeed = angularSpeed * dt * DEG2RAD;// Radians per frame

        const float angle = Angle(direction) * RAD2DEG;
        const Vector2 right = Rotate(direction, 90.0f * DEG2RAD);

        if (IsKeyDown(KEY_W))
            position = position + direction * frameLinearSpeed;
        if (IsKeyDown(KEY_S))
            position = position - direction * frameLinearSpeed;
        if (IsKeyDown(KEY_D))
            position = position + right * frameLinearSpeed;
        if (IsKeyDown(KEY_A))
            position = position - right * frameLinearSpeed;
        if (IsKeyDown(KEY_E))
            direction = Rotate(direction, frameAngularSpeed);
        if (IsKeyDown(KEY_Q))
            direction = Rotate(direction, -frameAngularSpeed);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectanglePro({ position.x, position.y, 60.0f, 30.0f }, { 30.0f, 15.0f }, angle, RED);
        DrawLineV(position, position + direction * 100.0f, BLACK);
        DrawLineV(position, position + right * 50.0f, BLACK);
        DrawLineV(position, position - right * 50.0f, BLACK);

        rlImGuiBegin();
        ImGui::SliderFloat("Linear Speed", &linearSpeed, 0.0f, 500.0f);
        ImGui::SliderFloat("Angular Speed", &angularSpeed, 0.0f, 360.0f);
        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}