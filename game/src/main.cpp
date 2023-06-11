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
    Vector2 velocity{ 100.0f, 0.0f };
    Vector2 acceleration{};

    Vector2 direction = Normalize(velocity);
    float angularSpeed = 200.0f;

    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();
        velocity = velocity + acceleration * dt;
        position = position + velocity * dt + acceleration * dt * dt * 0.5f;

        // Rotate at angular speed towards the direction of motion
        const float frameRotation = angularSpeed * DEG2RAD * dt;
        direction = RotateTowards(direction, Normalize(velocity), frameRotation);

        if (position.x >= SCREEN_WIDTH) position.x = 0.0f;
        else if (position.x <= 0.0f) position.x = SCREEN_WIDTH;
        if (position.y >= SCREEN_HEIGHT) position.y = 0.0f;
        else if (position.y <= 0.0f) position.y = SCREEN_HEIGHT;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(position, 25.0f, RED);
        DrawLineV(position, position + direction * 100.0f, BLACK);

        rlImGuiBegin();
        ImGui::SliderFloat2("Position", &position.x, -500.0f, 500.0f);
        ImGui::SliderFloat2("Velocity", &velocity.x, -500.0f, 500.0f);
        ImGui::SliderFloat2("Acceleration", &acceleration.x, -500.0f, 500.0f);
        ImGui::SliderFloat("Angular Speed", &angularSpeed, 0.0f, 360.0f);
        if (ImGui::Button("Reset"))
        {
            position = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
            velocity = {};
            acceleration = {};
        }
        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}