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

// Right perpendicular, ie if v = [0, 1] then PerpendicularL(v) = [1, 0]
Vector2 PerpendicularR(Vector2 v)
{
    return { -v.y, v.x };
}

// Right perpendicular, ie if v = [0, 1] then Perpendicular(v) = [-1, 0]
Vector2 PerpendicularL(Vector2 v)
{
    return { v.y, -v.x };
}

// Returns an array of perpendicular vectors to the polygon's edges
Points Normals(const Points& points)
{
    Points normals(points.size());
    for (size_t i = 0; i < points.size(); i++)
    {
        Vector2 p0 = points[i];
        Vector2 p1 = points[(i + 1) % points.size()];
        //normals[i] = Perpendicular(p0 - p1);
        normals[i] = PerpendicularL(p1 - p0);
    }
    return normals;
}

void Project(const Points& points, const Vector2& axis, float& min, float& max)
{
    for (size_t i = 0; i < points.size(); i++)
    {
        float t = Dot(axis, points[i]);
        if (t < min) min = t;
        if (t > max) max = t;
    }
}

bool Overlaps(float min1, float max1, float min2, float max2)
{
    return !((max1 < min2) || (max2 < min1));
}

float Overlap(float min1, float max1, float min2, float max2)
{
    if (Overlaps(min1, max1, min2, max2))
        return fminf(max1, max2) - fmaxf(min1, min2);
    return 0.0f;
}

// Draws lines from midpoints to midpoints + normals of a polygon
void DrawAxes(const Points& points, const Color& color)
{
    for (size_t i = 0; i < points.size(); i++)
    {
        Vector2 p0 = points[i];
        Vector2 p1 = points[(i + 1) % points.size()];
        Vector2 midpoint = (p0 + p1) * 0.5f;
        Vector2 normal = PerpendicularL(p1 - p0);
        DrawLineV(midpoint, midpoint + normal, color);
    }
}

bool CheckCollisionPolygons(const Points& points1, const Points& points2, Vector2* mtv = nullptr)
{
    // Against axes 1
    {
        float min1 = FLT_MAX, min2 = FLT_MAX;
        float max1 = FLT_MIN, max2 = FLT_MIN;
        for (const Vector2& axis : Normals(points1))
        {
            Project(points1, axis, min1, max1);
            Project(points2, axis, min2, max2);
            if (!Overlaps(min1, max1, min2, max2))
                return false;
        }
    }

    // Against axes 2
    {
        float min1 = FLT_MAX, min2 = FLT_MAX;
        float max1 = FLT_MIN, max2 = FLT_MIN;
        for (const Vector2& axis : Normals(points2))
        {
            Project(points1, axis, min1, max1);
            Project(points2, axis, min2, max2);
            if (!Overlaps(min1, max1, min2, max2))
                return false;
        }
    }

    return true;
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
    transform.translation = { SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.5f };
    transform.scale = 100.0f;

    Transform2 transform2;
    transform2.translation = { SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.5f };
    transform2.scale = 100.0f;

    float translationSpeed = 350.0f;
    float rotationSpeed = 250.0f * DEG2RAD;
    float scaleSpeed = 50.0f;
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        float translationDelta = translationSpeed * dt;
        float rotationDelta = rotationSpeed * dt;
        float scaleDelta = scaleSpeed * dt;
        //transform2.rotation += rotationDelta;

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
        Vector2 right = PerpendicularR(forward);
        if (IsKeyDown(KEY_A))
            transform.translation = transform.translation - right * translationDelta;
        if (IsKeyDown(KEY_D))
            transform.translation = transform.translation + right * translationDelta;

        if (IsKeyDown(KEY_UP))
            transform2.translation.y -= translationDelta;
        if (IsKeyDown(KEY_DOWN))
            transform2.translation.y += translationDelta;
        if (IsKeyDown(KEY_LEFT))
            transform2.translation.x -= translationDelta;
        if (IsKeyDown(KEY_RIGHT))
            transform2.translation.x += translationDelta;

        Points points = polygon;
        Apply(transform, points);

        Points points2 = polygon;
        Apply(transform2, points2);

        Vector2 mouse = GetMousePosition();
        //bool collision = CheckCollisionPointPoly(mouse, points.data(), points.size());
        bool collision = CheckCollisionPolygons(points, points2);
        Color color = collision ? RED : GREEN;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(mouse, 5.0f, DARKGRAY);

        DrawLineStrip(points.data(), points.size(), color);
        DrawLineStrip(points2.data(), points2.size(), color);

        DrawAxes(points, ORANGE);
        DrawAxes(points2, ORANGE);

        DrawText("SPACE / LSHIFT to scale up/down", 10, 10, 20, RED);
        DrawText("W / S to move forwards/backwards", 10, 30, 20, ORANGE);
        DrawText("D / A to move right/left", 10, 50, 20, BLUE);
        DrawText("E / Q to rotate right/left", 10, 70, 20, PURPLE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
