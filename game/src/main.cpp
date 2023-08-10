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

// Returns an array of normalized perpendicular vectors to the polygon's edges
Points Normals(const Points& points)
{
    Points normals(points.size());
    for (size_t i = 0; i < points.size(); i++)
    {
        Vector2 p0 = points[i];
        Vector2 p1 = points[(i + 1) % points.size()];
        normals[i] = PerpendicularL(p1 - p0);
        //normals[i] = PerpendicularL(Normalize(p1 - p0));
        // Not even sure why I need world-space normals,
        // but normalizing model-space normals affects the magnitude of world-space normals...
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

class Polygon
{
public:
    Polygon(Points&& points)
    {
        mVertices = std::move(points);
        mNormals = Normals(mVertices);
    }

    void Update()
    {
        if (dirty)
        {
            Matrix scale = Scale(transform.scale, transform.scale, 1.0f);
            Matrix rotate = RotateZ(transform.rotation);
            Matrix translate = Translate(transform.translation.x, transform.translation.y, 0.0f);
            Matrix worldMatrix = scale * rotate * translate;

            worldVertices = mVertices;
            for (Vector2& vertex : worldVertices)
                vertex = Multiply(vertex, worldMatrix);

            worldNormals = mNormals;
            for (Vector2& normal : worldNormals)
                normal = Multiply(normal, worldMatrix);

            // The normal matrix is not applicable here for 2 reasons:
            // 1. We only allow uniform scaling (uniform scale does NOT change direction of normals)
            // 2. We want world-space normals; The normal-matrix is typically 3x3 (no translation).
            // Furthermore, Transpose(Invert(scale * rotate)) removes the scale (rotate != world).

            dirty = false;
        }
    }

    void Render(const Color& color, float thick = 1.0f)
    {
        for (size_t i = 0; i < worldVertices.size(); i++)
        {
            Vector2 p0 = worldVertices[i];
            Vector2 p1 = worldVertices[(i + 1) % worldVertices.size()];
            DrawLineEx(p0, p1, thick, color);
        }
    }

    void RenderNormals(float thick = 5.0f)
    {
        // Render normals calculated from world-matrix at p0
        // Render normals calculated from world-vertices at p1
        for (size_t i = 0; i < worldVertices.size(); i++)
        {
            Vector2 p0 = worldVertices[i];
            Vector2 p1 = worldVertices[(i + 1) % worldVertices.size()];
            Vector2 n0 = worldNormals[i] - transform.translation;
            Vector2 n1 = PerpendicularL(p1 - p0);
            Vector2 t0 = Lerp(p0, p1, 0.45f);
            Vector2 t1 = Lerp(p0, p1, 0.55f);
            DrawLineEx(t0, t0 + n0, thick, DARKPURPLE);
            DrawLineEx(t1, t1 + n1, thick, BLUE);
            DrawCircleV(t0 + n0, thick, PURPLE);
            DrawCircleV(t1 + n1, thick, SKYBLUE);
        }

        // Render actual world position of normals
        for (const Vector2& normal : worldNormals)
        {
            DrawLineEx(transform.translation, normal, thick, ORANGE);
            DrawCircleV(normal, thick, GOLD);
        }
    }

    Transform2 transform;
    bool dirty = true;

    Points worldVertices;
    Points worldNormals;

private:
    Points mVertices;
    Points mNormals;
};

bool CheckCollisionPolygons(const Polygon& polygon1, const Polygon& polygon2, Vector2* mtv = nullptr)
{
    // Against axes 1
    {
        float min1 = FLT_MAX, min2 = FLT_MAX;
        float max1 = FLT_MIN, max2 = FLT_MIN;
        for (const Vector2& axis : polygon1.worldNormals)
        {
            Project(polygon1.worldVertices, axis, min1, max1);
            Project(polygon2.worldVertices, axis, min2, max2);
            if (!Overlaps(min1, max1, min2, max2))
                return false;
        }
    }

    // Against axes 2
    {
        float min1 = FLT_MAX, min2 = FLT_MAX;
        float max1 = FLT_MIN, max2 = FLT_MIN;
        for (const Vector2& axis : polygon2.worldNormals)
        {
            Project(polygon1.worldVertices, axis, min1, max1);
            Project(polygon2.worldVertices, axis, min2, max2);
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

    ///*
    Polygon polygon1
    ({
        { 1.0f, -1.0f },
        { 2.0f, 0.0f },
        { 1.0f, 1.0f },
        { -1.0f, 1.0f },
        { -1.0f, -1.0f }
    });
    
    Polygon polygon2
    ({
        { 1.0f, -1.0f },
        { 2.0f, 0.0f },
        { 1.0f, 1.0f },
        { -1.0f, 1.0f },
        { -1.0f, -1.0f }
    });//*/

    /*
    Polygon polygon1
    ({
        { 1.0f, -1.0f },
        { 1.0f, 1.0f },
        { -1.0f, 1.0f },
        { -1.0f, -1.0f }
    });

    Polygon polygon2
    ({
        { 1.0f, -1.0f },
        { 1.0f, 1.0f },
        { -1.0f, 1.0f },
        { -1.0f, -1.0f }
    });//*/

    Transform2& t1 = polygon1.transform;
    t1.translation = { SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.5f };
    //t1.rotation = -45.0f * DEG2RAD;
    t1.scale = 100.0f;
    
    Transform2& t2 = polygon2.transform;
    t2.translation = { SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.5f };
    //t2.rotation = -45.0f * DEG2RAD;
    t2.scale = 100.0f;

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
            t1.scale -= scaleDelta;
        if (IsKeyDown(KEY_SPACE))
            t1.scale += scaleDelta;

        // Rotate counter-clockwise/clockwise
        if (IsKeyDown(KEY_Q))
            t1.rotation -= rotationDelta;
        if (IsKeyDown(KEY_E))
            t1.rotation += rotationDelta;

        // Convert rotation to direction so it can be applied to translation
        Vector2 forward = Direction(t1.rotation);
        if (IsKeyDown(KEY_W))
            t1.translation = t1.translation + forward * translationDelta;
        if (IsKeyDown(KEY_S))
            t1.translation = t1.translation - forward * translationDelta;

        // Make a perpendicular vector by swapping y with x, then negating y
        Vector2 right = PerpendicularR(forward);
        if (IsKeyDown(KEY_A))
            t1.translation = t1.translation - right * translationDelta;
        if (IsKeyDown(KEY_D))
            t1.translation = t1.translation + right * translationDelta;

        if (IsKeyDown(KEY_UP))
            t2.translation.y -= translationDelta;
        if (IsKeyDown(KEY_DOWN))
            t2.translation.y += translationDelta;
        if (IsKeyDown(KEY_LEFT))
            t2.translation.x -= translationDelta;
        if (IsKeyDown(KEY_RIGHT))
            t2.translation.x += translationDelta;

        polygon1.dirty = polygon2.dirty = true;
        polygon1.Update();
        polygon2.Update();

        Vector2 mouse = GetMousePosition();
        bool collision = CheckCollisionPolygons(polygon1, polygon2);
        Color color = collision ? RED : GREEN;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(mouse, 5.0f, DARKGRAY);

        polygon1.Render(color, 5.0f);
        polygon2.Render(color, 5.0f);
        polygon1.RenderNormals();
        polygon2.RenderNormals();

        DrawText("SPACE / LSHIFT to scale up/down", 10, 10, 20, RED);
        DrawText("W / S to move forwards/backwards", 10, 30, 20, ORANGE);
        DrawText("D / A to move right/left", 10, 50, 20, BLUE);
        DrawText("E / Q to rotate right/left", 10, 70, 20, PURPLE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
