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

Vector2 Pursue(Vector2 target, Vector2 targetVelocity, Vector2 seeker, Vector2 seekerVelocity, float speed)
{
    return Seek(target + targetVelocity * Distance(target, seeker) / Length(seekerVelocity), seeker, seekerVelocity, speed);
}

Vector2 Evade(Vector2 target, Vector2 targetVelocity, Vector2 seeker, Vector2 seekerVelocity, float speed)
{
    return Flee(target + targetVelocity * Distance(target, seeker) / Length(targetVelocity), seeker, seekerVelocity, speed);
}

void DrawBody(Rigidbody rb, Color color)
{
    DrawCircleV(rb.pos, 25.0f, color);
    DrawLineV(rb.pos, rb.pos + rb.dir * 100.0f, BLACK);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);

    const Rectangle screen{ 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT };
    const Vector2 center{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    Rigidbody rb, target;
    rb.pos = target.pos = center;
    rb.angularSpeed = target.angularSpeed = 200.0f * DEG2RAD;
    rb.vel = { 100.0f, 0.0f };
    // Must have an initial velocity otherwise Pursue() & Evade() divide by zero

    while (!WindowShouldClose())
    {
        // t approaches 1 as seeker approaches target, otherwise t = 0 if outside radius
        const float evadeRadius = 500.0f;
        const float distance = Distance(rb.pos, target.pos);
        const float t = distance <= evadeRadius ? 1.0f - distance / evadeRadius : 0.0f;
        const float dt = GetFrameTime();

        // Seek cursor so Pursue() & Evade() have a velocity to predict with
        const float speed = 500.0f;
        target.acc = Seek(GetMousePosition(), target.pos, target.vel, speed);
        Update(target, dt);

        // Calculate both pursue and evade forces, then interpolate between them
        const Vector2 pursue = Pursue(target.pos, target.vel, rb.pos, rb.vel, speed);
        const Vector2 evade = Evade(target.pos, target.vel, rb.pos, rb.vel, speed);
        rb.acc = Lerp(pursue, evade, t);
        Update(rb, dt);

        // Reset to center if off screen
        rb.pos = CheckCollisionPointRec(rb.pos, screen) ? rb.pos : center;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        if (distance <= evadeRadius)
        {
            Color color = SKYBLUE;
            color.a = 255.0f * t;
            DrawCircleV(rb.pos, evadeRadius, color);
        }
        DrawBody(target, RED);
        DrawBody(rb, BLUE);
        DrawText("Blue circle will switch from pursuit to evasion the closer it gets to the red circle",
            10, 10, 20, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
