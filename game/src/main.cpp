#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

bool CheckCollisionLineCircle(Vector2 lineStart, Vector2 lineEnd, Vector2 circlePosition, float circleRadius)
{
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

// Acceleration-based avoid, nice because you can blend it with other steering forces
Vector2 Avoid(const Rigidbody& rb, const Vector2& obstaclePosition, float obstacleRadius,
    float dt, float probeAngle/*degrees*/, float probeLength)
{
    Vector2 probeEnd = rb.pos + Rotate(rb.dir, probeAngle * DEG2RAD) * probeLength;
    bool collision = CheckCollisionLineCircle(rb.pos, probeEnd, obstaclePosition, obstacleRadius);
    if (collision)
    {
        Vector2 vi = rb.vel;
        Vector2 linearDirection = Normalize(rb.vel);
        float linearSpeed = Length(rb.vel);
        float avoidSign = probeAngle >= 0.0f ? -1.0f : 1.0f;
        Vector2 vf = Rotate(linearDirection, rb.angularSpeed * avoidSign * dt) * linearSpeed;
        // a = (vf - vi) / t
        return (vf - vi) / dt;
    }
    return {};
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sunshine");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    float seekerProbeLength = 100.0f;
    float seekerRadius = 25.0f;
    Rigidbody seeker;
    seeker.pos = { SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.5f };
    seeker.dir = { 0.0f, 1.0f };
    seeker.angularSpeed = 360.0f * DEG2RAD;

    Vector2 obstaclePosition{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    float obstacleRadius = 50.0f;

    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();

        Vector2 leftEnd = seeker.pos + Rotate(seeker.dir, -30.0f * DEG2RAD) * seekerProbeLength;
        Vector2 rightEnd = seeker.pos + Rotate(seeker.dir, 30.0f * DEG2RAD) * seekerProbeLength;

        bool leftCollision = CheckCollisionLineCircle(seeker.pos, leftEnd, obstaclePosition, obstacleRadius);
        bool rightCollision = CheckCollisionLineCircle(seeker.pos, rightEnd, obstaclePosition, obstacleRadius);

        Vector2 avoidLeft = Avoid(seeker, obstaclePosition, obstacleRadius, dt, -30.0f, seekerProbeLength);
        Vector2 avoidRight = Avoid(seeker, obstaclePosition, obstacleRadius, dt, 30.0f, seekerProbeLength);
        Vector2 avoid = avoidLeft + avoidRight;

        seeker.acc = Seek(GetMousePosition(), seeker, 500.0f) + avoid;
        Update(seeker, dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(seeker.pos, seekerRadius, BLUE);
        DrawCircleV(obstaclePosition, obstacleRadius, GRAY);
        DrawLineV(seeker.pos, leftEnd, leftCollision ? RED : GREEN);
        DrawLineV(seeker.pos, rightEnd, rightCollision ? RED : GREEN);
        DrawText("I dare you to move through the grey circle ;)", 10, 10, 20, GRAY);
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
