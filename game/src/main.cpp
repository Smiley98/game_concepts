#include "rlImGui.h"
#include "Math.h"
#include <vector>
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
constexpr Vector2 CENTER { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };

struct Transform2
{
    Vector2 translation{};
    float rotation = 0.0f;
    float scale = 1.0f;
};

struct Range
{
    float min =  FLT_MAX;   // min is initialized to highest possible value
    float max = -FLT_MAX;   // max is initialized to lowest  possible value
};

using Ranges = std::vector<Range>;
using Points = std::vector<Vector2>;

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

// Returns an array of normalized counter-clockwise perpendicular vectors to the polygon's edges
Points Normals(const Points& points)
{
    Points normals(points.size());
    for (size_t i = 0; i < points.size(); i++)
    {
        Vector2 p0 = points[i];
        Vector2 p1 = points[(i + 1) % points.size()];
        normals[i] = Normalize(PerpendicularL(p1 - p0));
    }
    return normals;
}

// Determines the minimum and maximum scalar projection of all points onto a unit-length axis
void Project(const Points& points, const Vector2& axis, float& min, float& max)
{
    for (size_t i = 0; i < points.size(); i++)
    {
        float t = Dot(points[i], axis);
        if (t < min) min = t;
        if (t > max) max = t;
    }
}

// Scalar overlap test
bool Overlaps(float min1, float max1, float min2, float max2)
{
    return !((max1 < min2) || (max2 < min1));
}

// Scalar overlap difference
float Overlap(float min1, float max1, float min2, float max2)
{
    if (Overlaps(min1, max1, min2, max2))
        return fminf(max1, max2) - fmaxf(min1, min2);
    return 0.0f;
}

void DrawLineDotted(Vector2 start, Vector2 end, float thick, Color color, size_t subdivisions = 16);

class Polygon
{
public:
    Polygon(Points&& points) : axisCount(points.size())
    {
        mVertices = std::move(points);
        mNormals = Normals(mVertices);
    }

    void Update()
    {
        Matrix scale = Scale(transform.scale, transform.scale, 1.0f);
        Matrix rotate = RotateZ(transform.rotation);
        Matrix translate = Translate(transform.translation.x, transform.translation.y, 0.0f);
        Matrix worldMatrix = scale * rotate * translate;
        Matrix normalMatrix = Transpose(Invert(scale * rotate));

        worldVertices = mVertices;
        for (Vector2& vertex : worldVertices)
            vertex = Multiply(vertex, worldMatrix);

        worldNormals = mNormals;
        for (Vector2& normal : worldNormals)
            normal = Normalize(Multiply(normal, normalMatrix));
    }

    static void InitProjections(Polygon& polygon1, Polygon& polygon2)
    {
        polygon1.mProjectionsSelf.resize(polygon1.axisCount);
        polygon2.mProjectionsSelf.resize(polygon2.axisCount);
        polygon1.mProjectionsOther.resize(polygon2.axisCount);
        polygon2.mProjectionsOther.resize(polygon1.axisCount);
    }

    static void UpdateProjections(Polygon& polygon1, Polygon& polygon2)
    {
        std::fill(polygon1.mProjectionsSelf.begin(), polygon1.mProjectionsSelf.end(), Range{});
        std::fill(polygon2.mProjectionsSelf.begin(), polygon2.mProjectionsSelf.end(), Range{});
        std::fill(polygon1.mProjectionsOther.begin(), polygon1.mProjectionsOther.end(), Range{});
        std::fill(polygon2.mProjectionsOther.begin(), polygon2.mProjectionsOther.end(), Range{});

        for (size_t i = 0; i < polygon1.axisCount; i++)
        {
            Range& self = polygon1.mProjectionsSelf[i];
            Range& other = polygon2.mProjectionsOther[i];
            const Vector2& axis = polygon1.worldNormals[i];
            Project(polygon1.worldVertices, axis, self.min, self.max);
            Project(polygon2.worldVertices, axis, other.min, other.max);
        }
    
        for (size_t i = 0; i < polygon2.axisCount; i++)
        {
            Range& self = polygon2.mProjectionsSelf[i];
            Range& other = polygon1.mProjectionsOther[i];
            const Vector2& axis = polygon2.worldNormals[i];
            Project(polygon2.worldVertices, axis, self.min, self.max);
            Project(polygon1.worldVertices, axis, other.min, other.max);
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
        // Render normals (automatically) calculated from world-matrix at t0
        // Render normals (manually) calculated from world-vertices at t1
        for (size_t i = 0; i < worldVertices.size(); i++)
        {
            Vector2 p0 = worldVertices[i];
            Vector2 p1 = worldVertices[(i + 1) % worldVertices.size()];
            Vector2 n0 = worldNormals[i] * transform.scale;
            Vector2 n1 = Normalize(PerpendicularL(p1 - p0)) * transform.scale;
            Vector2 t0 = Lerp(p0, p1, 0.45f);
            Vector2 t1 = Lerp(p0, p1, 0.55f);
            DrawLineEx(t0, t0 + n0, thick, DARKPURPLE);
            DrawLineEx(t1, t1 + n1, thick, BLUE);
            DrawCircleV(t0 + n0, thick, PURPLE);
            DrawCircleV(t1 + n1, thick, SKYBLUE);
        }
    }

    static void RenderProjections(const Polygon& polygon1, const Polygon& polygon2)
    {
        auto renderProjection = [](const Vector2& axis, float min, float max, float offset, Color color)
        {
            Vector2 start = axis * min;
            Vector2 end = axis * max;
            Vector2 tangent = CENTER + PerpendicularR(axis) * offset;
            DrawLineEx(start + tangent, end + tangent, 4.0f, color);
        };

        const float offset1 = 115.0f;
        const float offset2 = 125.0f;
        const Color color1 = SKYBLUE;
        const Color color2 = PINK;

        for (size_t i = 0; i < polygon1.axisCount; i++)
        {
            const Vector2& axis = polygon1.worldNormals[i];
            const Range& self = polygon1.mProjectionsSelf[i];
            const Range& other = polygon2.mProjectionsOther[i];
            renderProjection(axis, self.min, self.max, offset1, color1);
            renderProjection(axis, other.min, other.max, offset2, color2);
        }

        for (size_t i = 0; i < polygon2.axisCount; i++)
        {
            const Vector2& axis = polygon2.worldNormals[i];
            const Range& self = polygon2.mProjectionsSelf[i];
            const Range& other = polygon1.mProjectionsOther[i];
            renderProjection(axis, self.min, self.max, offset2, color2);
            renderProjection(axis, other.min, other.max, offset1, color1);
        }
    }
    
    static void RenderAxes(const Polygon& polygon1, const Polygon& polygon2)
    {
        auto renderAxis = [](const Vector2& axis, Color color)
        {
            const float offset = 150.0f;
            const float length = 400.0f;
            Vector2 midpoint = CENTER + PerpendicularR(axis) * offset;
            Vector2 start = midpoint + axis * length;
            Vector2 end = midpoint - axis * length;
            DrawLineDotted(start, end, 4.0f, color, 64);
        };

        for (const Vector2& axis : polygon1.worldNormals)
            renderAxis(axis, DARKBLUE);

        for (const Vector2& axis : polygon2.worldNormals)
            renderAxis(axis, DARKPURPLE);
    }

    Transform2 transform;
    const size_t axisCount;

    Points worldVertices;
    Points worldNormals;

private:
    Points mVertices;
    Points mNormals;

    // For visualization only
    Ranges mProjectionsSelf;
    Ranges mProjectionsOther;
};

bool CheckCollisionPolygons(const Polygon& polygon1, const Polygon& polygon2, Vector2* mtv = nullptr)
{
    // Against axes 1
    {
        for (const Vector2& axis : polygon1.worldNormals)
        {
            float min1 = FLT_MAX, min2 = FLT_MAX;
            float max1 = -FLT_MAX, max2 = -FLT_MAX;
            Project(polygon1.worldVertices, axis, min1, max1);
            Project(polygon2.worldVertices, axis, min2, max2);
            if (!Overlaps(min1, max1, min2, max2))
                return false;
        }
    }

    // Against axes 2
    {
        for (const Vector2& axis : polygon2.worldNormals)
        {
            float min1 = FLT_MAX, min2 = FLT_MAX;
            float max1 = -FLT_MAX, max2 = -FLT_MAX;
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

    // Pentagons
    /*
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

    // Rectangles
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

    // Triangles
    ///*
    Polygon polygon1
    ({
        { 0.0f, -0.66f },
        { 0.5f, 0.33f },
        { -0.5f, 0.33f }
    });

    Polygon polygon2
    ({
        { 0.0f, -0.66f },
        { 0.5f, 0.33f },
        { -0.5f, 0.33f }
    });//*/

    Polygon::InitProjections(polygon1, polygon2);

    Transform2& t1 = polygon1.transform;
    t1.translation = { SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.5f };
    t1.scale = 100.0f;
    
    Transform2& t2 = polygon2.transform;
    t2.translation = { SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.5f };
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

        polygon1.Update();
        polygon2.Update();
        Polygon::UpdateProjections(polygon1, polygon2);

        Vector2 mouse = GetMousePosition();
        bool collision = CheckCollisionPolygons(polygon1, polygon2);
        Color color = collision ? RED : GREEN;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(mouse, 5.0f, DARKGRAY);

        polygon1.Render(color, 5.0f);
        polygon2.Render(color, 5.0f);
        //polygon1.RenderNormals();
        //polygon2.RenderNormals();
        Polygon::RenderAxes(polygon1, polygon2);
        Polygon::RenderProjections(polygon1, polygon2);

        DrawLineEx(t1.translation, t1.translation + forward * t1.scale, 5.0f, GRAY);

        DrawText("SPACE / LSHIFT to scale up/down", 10, 10, 20, RED);
        DrawText("W / S to move forwards/backwards", 10, 30, 20, ORANGE);
        DrawText("D / A to move right/left", 10, 50, 20, BLUE);
        DrawText("E / Q to rotate right/left", 10, 70, 20, PURPLE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void DrawLineDotted(Vector2 start, Vector2 end, float thick, Color color, size_t subdivisions)
{
    Vector2 delta = end - start;
    float t = 1.0f / subdivisions;
    for (size_t i = 0; i < subdivisions; i++)
    {
        if (i % 2 == 0)
        {
            float t0 = t * i;
            float t1 = t * (i + 1);
            Vector2 a = start + delta * t0;
            Vector2 b = start + delta * t1;
            DrawLineEx(start + delta * t0, start + delta * t1, thick, color);
        }
    }
}