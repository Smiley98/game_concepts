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

Vector2 Pursue(Vector2 target, Vector2 targetVelocity, Vector2 seeker, Vector2 seekerVelocity, float speed)
{
    // Determine how long it would take to reach the target at the current speed
    // (Speed = Distance / Time, so Time = Distance / Speed)
    const float travelTime = Distance(target, seeker) / Length(seekerVelocity);

    // Seek towards the target's future position (assuming it maintains course)
    Vector2 futurePosition = target + targetVelocity * travelTime;
    return Seek(futurePosition, seeker, seekerVelocity, speed);

    // Simplified form:
    //return Seek(target + targetVelocity * Distance(target, seeker) / Length(seekerVelocity), seeker, seekerVelocity, speed);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    Rigidbody rb;
    rb.pos = { SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.9f };
    rb.vel = { 100.0f, 0.0f };

    Rigidbody target;
    target.pos = { SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.5f };
    target.vel = { 100.0f, 0.0f };

    rb.angularSpeed = 200.0f * DEG2RAD;
    float linearSpeed = 0.0f;

    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();

        // Pursue at 150% of the target's velocity
        linearSpeed = Length(target.vel) * 1.5f;
        rb.acc = Pursue(target.pos, target.vel, rb.pos, rb.vel, linearSpeed);

        Update(rb, dt);
        Update(target, dt);

        // Reset if target goes off screen
        if (!CheckCollisionPointRec(target.pos, { 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT }))
        {
            target.pos = { SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.5f };
            rb.pos = { SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.9f };
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(rb.pos, 25.0f, BLUE);
        DrawCircleV(target.pos, 25.0f, RED);
        DrawLineV(rb.pos, rb.pos + rb.dir * 100.0f, BLACK);
        DrawLineV(target.pos, target.pos + target.dir * 100.0f, BLACK);
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}