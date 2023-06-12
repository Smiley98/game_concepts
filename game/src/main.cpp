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

Vector2 Evade(Vector2 target, Vector2 targetVelocity, Vector2 seeker, Vector2 seekerVelocity, float speed)
{
    // Determine how long it would take to reach the seeker at the current speed
    // (Speed = Distance / Time, so Time = Distance / Speed)
    const float travelTime = Distance(target, seeker) / Length(targetVelocity);

    // Flee towards the target's future position (assuming it maintains course)
    Vector2 futurePosition = target + targetVelocity * travelTime;
    return Flee(futurePosition, seeker, seekerVelocity, speed);

    // Simplified form:
    //return Flee(target + targetVelocity * Distance(target, seeker) / Length(targetVelocity), seeker, seekerVelocity, speed);
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

    const Vector2 center{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    const float orbitRadius = 250.0f;
    Rigidbody target;
    target.pos = center + Vector2{ orbitRadius, 0.0f };

    Rigidbody evader, fleer;
    evader.pos = fleer.pos = center;
    fleer.angularSpeed = evader.angularSpeed = target.angularSpeed = 360.0f * DEG2RAD;

    while (!WindowShouldClose())
    {
        const float t = GetTime();
        const float dt = GetFrameTime();
        const Vector2 orbit = center + Vector2{ cosf(t), sinf(t) } * orbitRadius;

        // Seek orbit because the target must have a velocity for evasion to work
        target.acc = Seek(orbit, target.pos, target.vel, orbitRadius);
        Update(target, dt);

        // Move at 25% of the target's velocity
        const float speed = Length(target.vel) * 0.25f;
        evader.acc = Evade(target.pos, target.vel, evader.pos, evader.vel, speed);
        fleer.acc = Flee(target.pos, fleer.pos, fleer.vel, speed);

        Update(evader, dt);
        Update(fleer, dt);

        // Reset if bodies go off screen
        const Rectangle screen{ 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT };
        evader.pos = CheckCollisionPointRec(evader.pos, screen) ? evader.pos : center;
        fleer.pos = CheckCollisionPointRec(fleer.pos, screen) ? fleer.pos : center;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawBody(target, GREEN);
        DrawBody(evader, PURPLE);
        DrawBody(fleer, BLUE);
        DrawText("Evader -- Purple", 10, 10, 20, PURPLE);
        DrawText("Fleer -- Blue", 10, 30, 20, BLUE);
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}