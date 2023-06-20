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

// Arrive = Seek + Decelerate
float Decelerate(Vector2 target, Vector2 seekerPosition, Vector2 seekerVelocity);

Vector2 Seek(Vector2 target, Vector2 seekerPosition, Vector2 seekerVelocity, float speed)
{
    // From seeker to target with a magnitude (strength) of speed
    Vector2 desiredVelocity = Normalize(target - seekerPosition) * speed;

    // Apply difference as an acceleration
    return desiredVelocity - seekerVelocity;
}

Vector2 Arrive(Vector2 target, Vector2 seekerPosition, Vector2 seekerVelocity,
    float speed)
{
    // Decelerate against the direction of motion
    Vector2 deceleration = Normalize(seekerVelocity) *
        Decelerate(target, seekerPosition, seekerVelocity);
    return Seek(target, seekerPosition, seekerVelocity, speed) + deceleration;
}

// Given initial velocity (vi), final velocity (vf), and distance (d),
// solve for acceleration (a) for which vf = 0 (arrival):
// 
// vf^2 = vi^2 + 2a(d)
// 0^2 = vi^2 + 2a(d)
// -vi^2 / 2d = a
float Decelerate(Vector2 target, Vector2 seekerPosition, Vector2 seekerVelocity)
{
    float viSqr = LengthSqr(seekerVelocity);
    float d2 = 2.0f * Distance(seekerPosition, target);
    return -viSqr / d2;

    // Simplified equation:
    //return -LengthSqr(seekerVelocity) / (2.0f * Distance(seekerPosition, target));
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

    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();
        rb.acc = Arrive(GetMousePosition(), rb.pos, rb.vel, linearSpeed);
        Update(rb, dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(rb.pos, 25.0f, RED);
        DrawLineV(rb.pos, rb.pos + rb.dir * 100.0f, BLACK);

        rlImGuiBegin();
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