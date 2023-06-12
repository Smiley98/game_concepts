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

void DrawBody(Rigidbody rb, Color color)
{
    DrawCircleV(rb.pos, 25.0f, color);
    DrawLineV(rb.pos, rb.pos + rb.dir * 100.0f, BLACK);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    Rigidbody pursuer, pursuerTarget, seeker, seekerTarget;
    pursuer.pos = { SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.45f };
    pursuerTarget.pos = { SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.15f };
    seeker.pos = { SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.95f };
    seekerTarget.pos = { SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.65f };
    seeker.vel = pursuer.vel = seekerTarget.vel = pursuerTarget.vel = { 100.0f, 0.0f };
    seeker.angularSpeed = pursuer.angularSpeed = seekerTarget.angularSpeed = pursuerTarget.angularSpeed = 200.0f * DEG2RAD;

    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();

        // Move at 150% of the target's velocity
        const float speed = Length(pursuerTarget.vel) * 1.5f;
        pursuer.acc = Pursue(pursuerTarget.pos, pursuerTarget.vel, pursuer.pos, pursuer.vel, speed);
        seeker.acc = Seek(seekerTarget.pos, seeker.pos, seeker.vel, speed);

        Update(pursuer, dt);
        Update(pursuerTarget, dt);
        Update(seeker, dt);
        Update(seekerTarget, dt);

        // Reset if target goes off screen
        if (!CheckCollisionPointRec(pursuerTarget.pos, { 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT }))
        {
            pursuer.pos = { SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.45f };
            pursuerTarget.pos = { SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.15f };
            seeker.pos = { SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.95f };
            seekerTarget.pos = { SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.65f };
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawBody(pursuer, BLUE);
        DrawBody(seeker, BLUE);
        DrawBody(pursuerTarget, RED);
        DrawBody(seekerTarget, RED);
        DrawText("Pursue", 10, 5, 20, GRAY);
        DrawText("Seek", 10, SCREEN_HEIGHT * 0.5f + 5.0f, 20, GRAY);
        DrawLineEx({ 0.0f, SCREEN_HEIGHT * 0.5f }, { SCREEN_WIDTH, SCREEN_HEIGHT * 0.5f }, 5.0f, BLACK);
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}