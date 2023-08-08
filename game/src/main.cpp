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

// Perform transformation directly since we're only transforming a single point
void Apply(const Transform2& transform, Vector2& point)
{
    point = Rotate((point * transform.scale), transform.rotation) + transform.translation;
}

// More efficient to store the transformation as a matrix when transforming multiple points
void Apply(const Transform2& transform, Points& points)
{
    Matrix mat =
        Scale(transform.scale, transform.scale, 1.0f)
        * RotateZ(transform.rotation)
        * Translate(transform.translation.x, transform.translation.y, 0.0f);

    for (Vector2& point : points)
        point = Multiply(point, mat);
}

// Return perpendicular vector to v
Vector2 Normal(Vector2 v)
{
    return { -v.y, v.x };
}

// Returns the normalized normals of the polygon
Points Axes(Points polygon)
{
    Points axes(polygon.size());
    for (size_t i = 0; i < polygon.size(); i++)
    {
        Vector2 p0 = polygon[i];
        Vector2 p1 = polygon[(i + 1) % polygon.size()];
        axes[i] = Normalize(Normal(p0 - p1));
    }
    return axes;
}

// Renders axes by applying the polygon's transform to the axes then translating them to the polygon's midpoints
void DrawAxes(const Transform2& transform, const Points& polygon, const Color& color)
{
    Points points = polygon;
    Apply(transform, points);
    Points axes = Axes(points);
    for (size_t i = 0; i < points.size(); i++)
    {
        Vector2 p0 = points[i];
        Vector2 p1 = points[(i + 1) % points.size()];
        Vector2 midpoint = (p0 + p1) * 0.5f;
        Vector2 endpoint = axes[i] * transform.scale;
        DrawLineV(midpoint, midpoint + endpoint, color);
    }
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);

    Points polygon
    {
        { 1.0f, -1.0f },
        { 2.0f, 0.0f },
        { 1.0f, 1.0f },
        { -1.0f, 1.0f },
        { -1.0f, -1.0f },
        { 1.0f, -1.0f },
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

        // Decrease/increase scale
        if (IsKeyDown(KEY_LEFT_SHIFT))
            transform.scale -= scaleDelta;
        if (IsKeyDown(KEY_SPACE))
            transform.scale += scaleDelta;

        // Rotate counter-clockwise/clockwise
        if (IsKeyDown(KEY_Q))
            transform.rotation -= rotationDelta;
        if (IsKeyDown(KEY_E))
            transform.rotation += rotationDelta;

        // Convert rotation to direction so it can be applied to translation
        Vector2 forward = Direction(transform.rotation);
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

        Points points = polygon;
        Apply(transform, points);
        Vector2 origin{};
        Apply(transform, origin);

        Vector2 mouse = GetMousePosition();
        bool collision = CheckCollisionPointPoly(mouse, points.data(), points.size());
        Color color = collision ? RED : GREEN;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(mouse, 5.0f, DARKGRAY);
        DrawCircleV(origin, 5.0f, DARKGRAY);
        DrawLineEx(origin, origin + forward * transform.scale, 5.0f, DARKGRAY);
        DrawLineStrip(points.data(), points.size(), color);
        DrawAxes(transform, polygon, ORANGE);
        DrawText("SPACE / LSHIFT to scale up/down", 10, 10, 20, RED);
        DrawText("W / S to move forwards/backwards", 10, 30, 20, ORANGE);
        DrawText("D / A to move right/left", 10, 50, 20, BLUE);
        DrawText("E / Q to rotate right/left", 10, 70, 20, PURPLE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
