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

Vector2 Arrive(Vector2 target, Vector2 seekerPosition, Vector2 seekerVelocity,
    float speed, float slowRadius)
{
    Vector2 deceleration = Normalize(seekerVelocity) *
        Decelerate(target, seekerPosition, seekerVelocity);

    Vector2 seek = Seek(target, seekerPosition, seekerVelocity, speed);
    Vector2 arrive = seek + deceleration;

    // Interpolate between seek and arrive based on distance (t >= 1 if outside slow radius)
    float t = Clamp(Distance(seekerPosition, target) / slowRadius, 0.0f, 1.0f);
    return Lerp(arrive, seek, t);
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

    Rigidbody rb1, rb2;
    rb1.pos = rb2.pos = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    rb1.vel = rb2.vel = { 100.0f, 0.0f };

    // User works in degrees, physics works in radians
    float angularSpeed = 200.0f;
    rb1.angularSpeed = rb2.angularSpeed = angularSpeed * DEG2RAD;
    float linearSpeed = 500.0f;
    float slowRadius = 250.0f;

    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();
        const Vector2 target = GetMousePosition();
        rb1.acc = Arrive(target, rb1.pos, rb1.vel, linearSpeed);
        rb2.acc = Arrive(target, rb2.pos, rb2.vel, linearSpeed, slowRadius);
        Update(rb1, dt);
        Update(rb2, dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Visualize slow radius
        if (Distance(rb2.pos, target) <= slowRadius)
        {
            float t = Distance(rb2.pos, target) / slowRadius;
            Color color = RED;
            color.a = 255.0f * t;
            DrawCircleV(rb2.pos, slowRadius, color);
        }

        DrawCircleV(rb1.pos, 25.0f, BLUE);
        DrawCircleV(rb2.pos, 25.0f, RED);
        DrawLineV(rb1.pos, rb1.pos + rb1.dir * 100.0f, BLACK);
        DrawLineV(rb2.pos, rb2.pos + rb2.dir * 100.0f, BLACK);

        // Blue circle is constantly being slowed down
        // Red circle only slows down when within the slow radius
        DrawText("Global Arrive (Blue)", 10, 10, 20, BLUE);
        DrawText("Proximity Arrive (Red)", 10, 30, 20, RED);

        rlImGuiBegin();
        ImGui::SliderFloat("Slow Radius", &slowRadius, 0.0f, 1000.0f);
        ImGui::SliderFloat("Linear Speed", &linearSpeed, 0.0f, 1000.0f);
        if (ImGui::SliderFloat("Angular Speed", &angularSpeed, 0.0f, 360.0f))
            rb1.angularSpeed = rb2.angularSpeed = angularSpeed * DEG2RAD;
        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}