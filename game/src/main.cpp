#include "rlImGui.h"
#include "Math.h"
#include <vector>
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

using Points = std::vector<Vector2>;

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

// Return the next point given the current point (index)
Vector2 Next(const Points& points, size_t index)
{
    return points[(index + 1) % points.size()];
}

// Percentage of point P along line segment AB (0.0 = 0%, 1.0 = 100%. Can exceed 100%)
float Percent(Vector2 A, Vector2 B, Vector2 P)
{
    const Vector2 AB = B - A;
    const float t = Dot(P - A, AB) / LengthSqr(AB);
    return t;
}

Vector2 FollowLine(const Points& points, size_t& index,
    Vector2 seekerPosition, Vector2 seekerVelocity, float speed, float lookAheadDistance)
{
    const Vector2 current = points[index];
    const Vector2 next = Next(points, index);
    const Vector2 proj = ProjectPointLine(current, next, seekerPosition);
    const Vector2 ahead = proj + Normalize(next - current) * lookAheadDistance;
    const float percent = Percent(current, next, ahead);
    index = percent > 1.0f ? (index + 1) % points.size() : index;
    return Seek(ahead, seekerPosition, seekerVelocity, speed);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    size_t index = 0;
    Points points
    {
        { SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.25f },
        { SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.25f },
        { SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.75f },
        { SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.75f },
    };

    Rigidbody rb;
    rb.pos = points[index];

    float lookAheadDistance = 100.0f;
    float linearSpeed = 500.0f;
    float angularSpeed = 200.0f;
    rb.angularSpeed = angularSpeed * DEG2RAD;

    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();
        rb.acc = FollowLine(points, index, rb.pos, rb.vel, linearSpeed, lookAheadDistance);
        Update(rb, dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(rb.pos, 25.0f, RED);
        DrawLineV(rb.pos, rb.pos + rb.dir * 100.0f, BLACK);

        for (size_t i = 0; i < points.size(); i++)
            DrawLineEx(points[i], Next(points, i), 10.0f, GRAY);

        rlImGuiBegin();
        ImGui::SliderFloat("Look-Ahead", &lookAheadDistance, 50.0f, 250.0f);
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