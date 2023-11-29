#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Assuming mass of B is infinite
Vector2 CollisionVelocity(Vector2 vi, Vector2 n, float cf, float cr, Vector2& impulse, Vector2& friction)
{
    Vector2 velBA = vi; // (a.vel - b.vel, b = 0 so velBA = a)
    float t = Dot(velBA, n);
    if (t > 0.0f) return Vector2Zero();

    float j = -(1.0f + cr) * t;
    impulse = vi + n * j;

    Vector2 tangent = Normalize(velBA - n * t); // Perpendicular to the normal
    float jt = -Dot(velBA, tangent);            // How similar velocity is to tangent (* -1 to point backwards since friction)
    // ie if very similar, then lots of friction. Otherwise, velocity is very similar to normal, so not much friction
    jt = Clamp(jt, -j * cf, j * cf);
    friction = tangent * jt;

    return impulse + friction;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);

    int state = 0;  // 0 = none, 1 = velocity, 2 = normal
    const Vector2 center{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    const float radius = 250.0f;

    float speed = 1.0f;
    Vector2 direction = { 1.0f, 0.0f };
    Vector2 normal = { 0.0f, 1.0f };
    float cf = 1.0f;
    float cr = 1.0f;
    
    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_SPACE))
            ++state %= 3;

        if (state == 1)
            direction = Normalize(center - GetMousePosition());

        if (state == 2)
            normal = Normalize(GetMousePosition() - center);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        const Vector2 top = center + Vector2{ 0.0f, -radius };
        const Vector2 bot = center + Vector2{ 0.0f, radius };
        const Vector2 left = center + Vector2{ -radius, 0.0f };
        const Vector2 right = center + Vector2{ radius, 0.0f };
        Vector2 vi = direction * speed;
        Vector2 impulse, friction;
        Vector2 vf = CollisionVelocity(vi, normal, cf, cr, impulse, friction);

        DrawCircleV(center, radius, RED);
        DrawLineEx(top, bot, 5.0f, BLACK);
        DrawLineEx(left, right, 5.0f, BLACK);
        DrawLineEx(center, center + direction * speed * radius, 10.0f, GOLD);
        DrawLineEx(center, center + normal * radius, 5.0f, GREEN);
        DrawLineEx(center, center + impulse * radius, 5.0f, BLUE);
        DrawLineEx(center, center + friction * radius, 5.0f, PURPLE);

        const int fontSize = 20;
        const char* text0 = "0";
        const char* text90 = "90";
        const char* text180 = "180";
        const char* text270 = "270";
        const int size0 = MeasureText(text0, fontSize);
        const int size90 = MeasureText(text90, fontSize);
        const int size180 = MeasureText(text180, fontSize);
        const int size270 = MeasureText(text270, fontSize);
        DrawText(text0, right.x + size0, right.y - fontSize / 2, fontSize, DARKGRAY);
        DrawText(text90, bot.x - size90 / 2, bot.y + fontSize / 2, fontSize, DARKGRAY);
        DrawText(text180, left.x - (size180 + fontSize / 2), left.y - fontSize / 2, fontSize, DARKGRAY);
        DrawText(text270, top.x - size270 / 2, top.y - (3 * fontSize / 2), fontSize, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
