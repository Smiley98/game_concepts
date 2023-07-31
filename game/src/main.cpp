#include "rlImGui.h"
#include "Math.h"
#include <array>
#include <vector>
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

struct Timer
{
    float duration = 0.0f;
    float elapsed = 0.0f;

    bool Expired() { return elapsed >= duration; }
    void Reset() { elapsed = 0.0f; }
    void Tick(float dt) { elapsed += dt; }
};

struct Rigidbody
{
    Vector2 pos{};
    Vector2 vel{};
    Vector2 acc{};

    Vector2 dir{ 1.0f, 0.0f };  // right
    float angularSpeed = 0.0f;  // radians
};

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

size_t Nearest(Vector2 position, std::vector<Vector2> positions)
{
    size_t index = 0;
    for (size_t i = 1; i < positions.size(); i++)
    {
        if (DistanceSqr(position, positions[i]) < DistanceSqr(position, positions[index]))
            index = i;
    }
    return index;
}

enum State
{
    CLOCKWISE,
    IDLE,
    COUNTER_CLOCKWISE
};

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    std::vector<Vector2> points {
        { SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.25f },
        { SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.25f },
        { SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.75f },
        { SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.75f },
    };

    Rigidbody rb;
    rb.pos = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    rb.angularSpeed = 200.0f * DEG2RAD;
    size_t index = 0;

    // Change states every 5 seconds
    std::array<Timer, 3> timers;
    timers[CLOCKWISE].duration = timers[COUNTER_CLOCKWISE].duration = 5.0f;
    timers[IDLE].duration = 1.0f;
    State state = CLOCKWISE;

    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();
        timers[state].Tick(dt);
        if (timers[state].Expired())    // <-- State-specific timeout conditions
        {
            switch (state)              // <-- State-specific transitions
            {
            case CLOCKWISE:
            case COUNTER_CLOCKWISE:
                index = Nearest(rb.pos, points);
                state = IDLE;
                break;

            case IDLE:
                state = rand() % 2 == 0 ? CLOCKWISE : COUNTER_CLOCKWISE;
                break;
            }
            timers[state].Reset();
        }

        // State-specific updates
        if (CheckCollisionPointCircle(rb.pos, points[index], 100.0f))
        {
            switch (state)
            {
            case CLOCKWISE:
                ++index %= points.size();
                break;

            case COUNTER_CLOCKWISE:
                index = (index - 1 + points.size()) % points.size();
                break;
            }
        }

        // Additional state-specific updates
        if (state != IDLE)
        {
            // Seek next point if not idle
            rb.acc = Seek(points[index], rb.pos, rb.vel, 500.0f);
        }
        else
        {
            // Reset acceleration and apply friction (dampen velocity)
            rb.acc = {};
            rb.vel = rb.vel * 0.95f;
        }
        Update(rb, dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(rb.pos, 25.0f, BLUE);
        DrawLineV(rb.pos, rb.pos + rb.dir * 100.0f, BLACK);
        for (const Vector2& point : points)
            DrawCircleV(point, 20.0f, GRAY);
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
