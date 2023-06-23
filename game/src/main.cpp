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

Vector2 RandomCirclePoint(float radius)
{
    float unitRandom = Random(0.0f, 1.0f);
    float randomPoint = radius * sqrtf(unitRandom);
    float theta = Random(0.0f, 1.0f) * 2.0f * PI;
    return { randomPoint * cosf(theta), randomPoint * sinf(theta) };
}

// Seek random point ahead of seeker
// (Doesn't seem to be working as intended, seeking a completely random position suffices)
Vector2 Wander(Vector2 seekerPosition, Vector2 seekerVelocity, float speed, float radius)
{
    Vector2 point = RandomCirclePoint(radius);
    Vector2 target = seekerPosition + Normalize(seekerVelocity) * radius * 2.0f + point;
    return Seek(target, seekerPosition, seekerVelocity, speed);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    const Vector2 center{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    Rigidbody rb;
    rb.pos = center;

    // User works in degrees, physics works in radians
    float angularSpeed = 200.0f;
    rb.angularSpeed = angularSpeed * DEG2RAD;
    float linearSpeed = 50.0f;

    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();
        rb.acc = Wander(rb.pos, rb.vel, linearSpeed, 100.0f);
        Update(rb, dt);

        if (rb.pos.x <= 0.0f || rb.pos.x >= SCREEN_WIDTH ||
            rb.pos.y <= 0.0f || rb.pos.y >= SCREEN_HEIGHT)
        {
            rb.pos = center;
            rb.vel = { 0.0f, 0.0f };
        }

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
