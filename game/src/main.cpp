#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Structure containing kinematics physics data, technically a "Particle".
// In most game engines, a "Rigidbody" also has a mass and a collider.
struct Rigidbody
{
    Vector2 pos{};
    Vector2 vel{};
    Vector2 acc{};

    Vector2 dir{ 1.0f, 0.0f };  // right
    float angularSpeed = 0.0f;  // radians
};

// Kinematic physics update
void Update(Rigidbody& rb, float dt)
{
    rb.vel = rb.vel + rb.acc * dt;
    rb.pos = rb.pos + rb.vel * dt + rb.acc * dt * dt * 0.5f;
    rb.dir = RotateTowards(rb.dir, Normalize(rb.vel), rb.angularSpeed * dt);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    Rigidbody rb;
    rb.pos = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    rb.vel = { 100.0f, 0.0f };

    // User works in degrees, physics works in radians
    float angularSpeed = 200.0f;
    rb.angularSpeed = angularSpeed * DEG2RAD;

    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();
        Update(rb, dt);

        if (rb.pos.x >= SCREEN_WIDTH) rb.pos.x = 0.0f;
        else if (rb.pos.x <= 0.0f) rb.pos.x = SCREEN_WIDTH;
        if (rb.pos.y >= SCREEN_HEIGHT) rb.pos.y = 0.0f;
        else if (rb.pos.y <= 0.0f) rb.pos.y = SCREEN_HEIGHT;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(rb.pos, 25.0f, RED);
        DrawLineV(rb.pos, rb.pos + rb.dir * 100.0f, BLACK);

        rlImGuiBegin();
        ImGui::SliderFloat2("Position", &rb.pos.x, -500.0f, 500.0f);
        ImGui::SliderFloat2("Velocity", &rb.vel.x, -500.0f, 500.0f);
        ImGui::SliderFloat2("Acceleration", &rb.acc.x, -500.0f, 500.0f);

        if (ImGui::SliderFloat("Angular Speed", &angularSpeed, 0.0f, 360.0f))
            rb.angularSpeed = angularSpeed * DEG2RAD;

        if (ImGui::Button("Reset"))
        {
            rb.pos = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
            rb.vel = {};
            rb.acc = {};
        }
        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}