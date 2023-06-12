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

Vector2 Seek(Vector2 target, Vector2 seekerPosition, Vector2 seekerVelocity, float speed)
{
    return Normalize(target - seekerPosition) * speed - seekerVelocity;
}

Vector2 Flee(Vector2 target, Vector2 seekerPosition, Vector2 seekerVelocity, float speed)
{
    return Normalize(seekerPosition - target) * speed - seekerVelocity;
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
    float linearSpeed = 500.0f;

    // Interpolation parameter (0 = 0%, 1 = 100%, 0.25 = 25%, etc...)
    float t = 0.0f;

    while (!WindowShouldClose())
    {
        // Increase/decrease interpolation parameter with time and ensure range [0, 1]
        const float dt = GetFrameTime();
        if (IsKeyDown(KEY_E)) t += dt;
        if (IsKeyDown(KEY_Q)) t -= dt;
        t = Clamp(t, 0.0f, 1.0f);

        // Calculate both seek and flee forces, then interpolate between them
        const Vector2 seek = Seek(GetMousePosition(), rb.pos, rb.vel, linearSpeed);
        const Vector2 flee = Flee(GetMousePosition(), rb.pos, rb.vel, linearSpeed);
        rb.acc = Lerp(seek, flee, t);
        Update(rb, dt);

        // Reset seeker to center if it travels off screen
        if (rb.pos.x >= SCREEN_WIDTH || rb.pos.x <= 0.0f ||
            rb.pos.y >= SCREEN_HEIGHT || rb.pos.y <= 0.0f)
            rb.pos = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(rb.pos, 25.0f, RED);
        DrawLineV(rb.pos, rb.pos + rb.dir * 100.0f, BLACK);
        DrawText("E/Q or drag slider to interpolate between seek and flee", 10, 10, 20, DARKGRAY);

        rlImGuiBegin();
        ImGui::SliderFloat("Interpolation", &t, 0.0f, 1.0f);
        ImGui::SliderFloat("Linear Speed", &linearSpeed, 0.0f, 1000.0f);
        if (ImGui::SliderFloat("Angular Speed", &angularSpeed, 0.0f, 360.0f))
            rb.angularSpeed = angularSpeed * DEG2RAD;
        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}