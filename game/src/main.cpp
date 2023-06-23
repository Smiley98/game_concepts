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

// Accelerate towards target
Vector2 Seek(Vector2 target, Vector2 seekerPosition, Vector2 seekerVelocity, float speed)
{
    return Normalize(target - seekerPosition) * speed - seekerVelocity;
}

// Apply the difference between target velocity and seeker velocity as acceleration
Vector2 MatchVelocity(Vector2 targetVelocity, Vector2 seekerVelocity, float scalar = 1.0f)
{
    // Multiply by scalar to increase the rate at which velocities match
    return (targetVelocity - seekerVelocity) * scalar;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    Rigidbody seeker, matcher;
    seeker.pos = { SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.5f };
    matcher.pos = { SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.5f };

    float angularSpeed = 200.0f;
    seeker.angularSpeed = matcher.angularSpeed = angularSpeed * DEG2RAD;
    float linearSpeed = 500.0f;
    float scalar = 10.0f;

    while (!WindowShouldClose())
    {
        // Matcher will nearly mirror seeker's movement!
        const float dt = GetFrameTime();
        seeker.acc = Seek(GetMousePosition(), seeker.pos, seeker.vel, linearSpeed);
        matcher.acc = MatchVelocity(seeker.vel, matcher.vel, scalar);
        Update(seeker, dt);
        Update(matcher, dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(seeker.pos, 25.0f, RED);
        DrawCircleV(matcher.pos, 25.0f, BLUE);
        DrawLineV(seeker.pos, seeker.pos + seeker.dir * 100.0f, BLACK);
        DrawLineV(matcher.pos, matcher.pos + matcher.dir * 100.0f, BLACK);

        rlImGuiBegin();
        ImGui::SliderFloat("Match Strength", &linearSpeed, 1.0f, 10.0f);
        ImGui::SliderFloat("Linear Speed", &linearSpeed, 0.0f, 1000.0f);
        if (ImGui::SliderFloat("Angular Speed", &angularSpeed, 0.0f, 360.0f))
            seeker.angularSpeed = matcher.angularSpeed = angularSpeed * DEG2RAD;
        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
