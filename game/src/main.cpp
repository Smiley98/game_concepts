#include "rlImGui.h"
#include "Math.h"
#include <vector>
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

using Points = std::vector<Vector2>;

struct Transform2
{
    Vector2 translation{};
    float rotation = 0.0f;
    float scale = 1.0f;
};

void Apply(const Transform2& transform, Points& points)
{
    for (Vector2& point : points)
        point = Rotate((point * transform.scale), transform.rotation) + transform.translation;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);

    // Define our polygon relative to the origin [0, 0], then translate/rotate/scale accordingly!
    Points hexagon
    {
        { -0.5f, -0.33f },  // top left
        {  0.0f, -1.66f },  // top middle
        {  0.5f, -0.33f },  // top right
        {  0.5f,  0.33f },  // bottom left
        {  0.0f,  1.66f },  // bottom middle
        { -0.5f,  0.33f },  // bottom right
        { -0.5f, -0.33f }   // connect end to start
        // (necessary for existing collsion & rendering functions)
    };

    Transform2 transform;
    transform.translation = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    transform.scale = 100.0f;

    float translationSpeed = 350.0f;
    float rotationSpeed = 250.0f * DEG2RAD;
    float scaleSpeed = 50.0f;
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        float translationDelta = translationSpeed * dt;
        float rotationDelta = rotationSpeed * dt;
        float scaleDelta = scaleSpeed * dt;

        if (IsKeyDown(KEY_LEFT_SHIFT))
            transform.scale -= scaleDelta;
        if (IsKeyDown(KEY_SPACE))
            transform.scale += scaleDelta;

        if (IsKeyDown(KEY_Q))
            transform.rotation -= rotationDelta;
        if (IsKeyDown(KEY_E))
            transform.rotation += rotationDelta;

        // By default, directions are relative to the horizontal,
        // so subtract 90 degrees to rotate relative to the vertical
        Vector2 forward = Direction(transform.rotation - 90.0f * DEG2RAD);
        if (IsKeyDown(KEY_W))
            transform.translation = transform.translation + forward * translationDelta;
        if (IsKeyDown(KEY_S))
            transform.translation = transform.translation - forward * translationDelta;

        // Make a perpendicular vector by swapping y with x, then negating y
        Vector2 right = { -forward.y, forward.x };
        if (IsKeyDown(KEY_A))
            transform.translation = transform.translation - right * translationDelta;
        if (IsKeyDown(KEY_D))
            transform.translation = transform.translation + right * translationDelta;

        Points points = hexagon;
        Apply(transform, points);

        Vector2 mouse = GetMousePosition();
        bool collision = CheckCollisionPointPoly(mouse, points.data(), points.size());
        Color color = collision ? RED : GREEN;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(mouse, 5.0f, DARKGRAY);
        DrawLineStrip(points.data(), points.size(), color);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
