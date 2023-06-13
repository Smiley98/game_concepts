#include "rlImGui.h"
#include "Math.h"
#include <array>
using std::array;
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

bool CheckCollisionLineCircle(Vector2 lineStart, Vector2 lineEnd, Vector2 circlePosition, float circleRadius)
{
    // Project circle center onto line, then treat as point-circle collision
    Vector2 nearest = NearestPoint(lineStart, lineEnd, circlePosition);
    return DistanceSqr(nearest, circlePosition) <= circleRadius * circleRadius;
}

struct Rigidbody
{
    Vector2 pos{};
    Vector2 vel{};
    Vector2 acc{};

    Vector2 dir{};
    float angularSpeed;
};

struct Probe
{
    float angle = 0.0f;
    float length = 0.0f;
};

void Update(Rigidbody& rb, float dt)
{
    rb.vel = rb.vel + rb.acc * dt;
    rb.pos = rb.pos + rb.vel * dt + rb.acc * dt * dt * 0.5f;
    rb.dir = RotateTowards(rb.dir, Normalize(rb.vel), rb.angularSpeed * dt);
}

Vector2 Seek(const Vector2& pos, const Rigidbody& rb, float maxSpeed)
{
    return Normalize(pos - rb.pos) * maxSpeed - rb.vel;
}

Vector2 Avoid(const Rigidbody& rb, float probeAngle, float dt)
{
    // Steer away from the obstacle (right or left depending on sign of probe angle)
    // Store the magnitude of the initial velocity (vi), rotate the direction vector,
    // then restore the magnitude by multiplying it by the corrected direction.
    // Finally, solve for acceleration using a = (vf - vi) / t
    Vector2 vi = rb.vel;
    Vector2 linearDirection = Normalize(rb.vel);
    float linearSpeed = Length(rb.vel);
    float avoidSign = probeAngle >= 0.0f ? -1.0f : 1.0f;
    Vector2 vf = Rotate(linearDirection, rb.angularSpeed * avoidSign * dt) * linearSpeed;
    return (vf - vi) / dt;
}

Vector2 ProbeEnd(const Rigidbody& rb, const Probe& probe)
{
    return rb.pos + Rotate(rb.dir, probe.angle * DEG2RAD) * probe.length;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    SetTargetFPS(60);

    Rigidbody seeker;
    seeker.pos = { SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.5f };
    seeker.dir = { 0.0f, 1.0f };
    seeker.angularSpeed = 360.0f * DEG2RAD;

    Vector2 obstaclePosition{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    float obstacleRadius = 50.0f;

    array<Probe, 4> probes
    {
        Probe{ -30.0f, 100.0f },
        Probe{ -15.0f, 200.0f },
        Probe{  15.0f, 200.0f },
        Probe{  30.0f, 100.0f },
    };

    array<Vector2, 4> ends{};
    array<bool, 4> collisions{};

    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();

        Vector2 avoidance{};
        for (size_t i = 0; i < probes.size(); i++)
        {
            ends[i] = ProbeEnd(seeker, probes[i]);
            collisions[i] = CheckCollisionLineCircle(seeker.pos, ends[i], obstaclePosition, obstacleRadius);
            if (collisions[i])
                avoidance = avoidance + Avoid(seeker, probes[i].angle, dt);
        }

        seeker.acc = Seek(GetMousePosition(), seeker, 500.0f) + avoidance;
        Update(seeker, dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(seeker.pos, 25.0f, BLUE);
        DrawCircleV(obstaclePosition, obstacleRadius, GRAY);
        for (size_t i = 0; i < probes.size(); i++)
            DrawLineV(seeker.pos, ends[i], collisions[i] ? RED : GREEN);
        DrawText("I dare you to move through the grey circle ;)", 10, 10, 20, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
