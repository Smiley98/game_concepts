#include "rlImGui.h"
#include "Math.h"
#include <vector>

// Dimensions must match in order for projection renders to work
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
constexpr Vector2 CENTER { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };

struct Transform2
{
    Vector2 translation{};
    float rotation = 0.0f;
    float scale = 1.0f;
};

using Points = std::vector<Vector2>;
struct Polygon
{
    // Transformation & number of points to transform
    Transform2 transform;
    size_t count = 0;

    // Original vertices & original normals
    Points modelVertices;
    Points modelNormals;

    // Transformed vertices & transformed normals
    Points worldVertices;
    Points worldNormals;
};

// Right (clockwise) perpendicular, ie if v = [0, 1] then PerpendicularL(v) = [1, 0]
Vector2 PerpendicularR(Vector2 v)
{
    return { -v.y, v.x };
}

// Left (counter-clockwise) perpendicular, ie if v = [0, 1] then Perpendicular(v) = [-1, 0]
Vector2 PerpendicularL(Vector2 v)
{
    return { v.y, -v.x };
}

// Returns an array of normalized left (CCW) perpendicular vectors to the polygon's edges
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

// Initializes a polygon's vertices & normals
void Create(Polygon& polygon, const Points& vertices)
{
    polygon.modelVertices = vertices;
    polygon.modelNormals = Normals(vertices);

    polygon.count = vertices.size();
    polygon.worldVertices.resize(polygon.count);
    polygon.worldNormals.resize(polygon.count);
}

// Updates a polygon's world-vertices & world-normals
void Update(Polygon& polygon)
{
    Transform2& transform = polygon.transform;
    Matrix scale = Scale(transform.scale, transform.scale, 1.0f);
    Matrix rotate = RotateZ(transform.rotation);
    Matrix translate = Translate(transform.translation.x, transform.translation.y, 0.0f);
    Matrix worldMatrix = scale * rotate * translate;
    Matrix normalMatrix = Transpose(Invert(scale * rotate));

    for (size_t i = 0; i < polygon.count; i++)
    {
        polygon.worldVertices[i] = Multiply(polygon.modelVertices[i], worldMatrix);
        polygon.worldNormals[i] = Normalize(Multiply(polygon.modelNormals[i], normalMatrix));
    }
}

// Draws a polygon's wireframes
void Render(const Polygon& polygon, const Color& color, float thick = 1.0f)
{
    for (size_t i = 0; i < polygon.count; i++)
    {
        Vector2 p0 = polygon.worldVertices[i];
        Vector2 p1 = polygon.worldVertices[(i + 1) % polygon.count];
        DrawLineEx(p0, p1, thick, color);
    }
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
    return fminf(max1, max2) - fmaxf(min1, min2);
}

// Resolve polygon-polygon collision using SAT (Separation of Axes Theorem)!
bool CheckCollisionPolygons(const Polygon& polygon1, const Polygon& polygon2, Vector2* mtv = nullptr)
{
    float collisionDepth = FLT_MAX;
    Vector2 collisionNormal{};

    // Check if world-space vertices overlap along polygon 1's world-space axes
    {
        for (const Vector2& axis : polygon1.worldNormals)
        {
            float min1 = FLT_MAX, min2 = FLT_MAX;
            float max1 = -FLT_MAX, max2 = -FLT_MAX;
            Project(polygon1.worldVertices, axis, min1, max1);
            Project(polygon2.worldVertices, axis, min2, max2);
            if (!Overlaps(min1, max1, min2, max2))
                return false;
            else
            {
                float depth = Overlap(min1, max1, min2, max2);
                if (depth < collisionDepth)
                {
                    collisionDepth = depth;
                    collisionNormal = axis;
                }
            }
        }
    }

    // Check if world-space vertices overlap along polygon 2's world-space axes
    {
        for (const Vector2& axis : polygon2.worldNormals)
        {
            float min1 = FLT_MAX, min2 = FLT_MAX;
            float max1 = -FLT_MAX, max2 = -FLT_MAX;
            Project(polygon1.worldVertices, axis, min1, max1);
            Project(polygon2.worldVertices, axis, min2, max2);
            if (!Overlaps(min1, max1, min2, max2))
                return false;
            else
            {
                float depth = Overlap(min1, max1, min2, max2);
                if (depth < collisionDepth)
                {
                    collisionDepth = depth;
                    collisionNormal = axis;
                }
            }
        }
    }

    // Ensure MTV resolves polygon1 from polygon2 (negate if normal points towards polygon 2)
    if (mtv != nullptr)
    {
        Vector2 AB = polygon2.transform.translation - polygon1.transform.translation;
        *mtv = Dot(AB, collisionNormal) < 0.0f ?
            collisionNormal * collisionDepth : collisionNormal * -collisionDepth;
    }

    return true;
}

/*
 * Visualization Begin *
 */
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

struct Projection
{
    float min = FLT_MAX;
    float max = -FLT_MAX;
};

struct Projections
{
    Polygon* polygon1 = nullptr;
    Polygon* polygon2 = nullptr;
    std::vector<Projection> proj11;  // 1 onto 1
    std::vector<Projection> proj12;  // 1 onto 2
    std::vector<Projection> proj21;  // 2 onto 1
    std::vector<Projection> proj22;  // 2 onto 2
};

void Create(Projections& projections, const Polygon& polygon1, const Polygon& polygon2)
{
    projections.polygon1 = (Polygon*)&polygon1;
    projections.polygon2 = (Polygon*)&polygon2;
    projections.proj11.resize(polygon1.count);
    projections.proj21.resize(polygon1.count);
    projections.proj12.resize(polygon2.count);
    projections.proj22.resize(polygon2.count);
}

void Update(Projections& projections)
{
    const Polygon& p1 = *projections.polygon1;
    const Polygon& p2 = *projections.polygon2;
    std::fill(projections.proj11.begin(), projections.proj11.end(), Projection{});
    std::fill(projections.proj12.begin(), projections.proj12.end(), Projection{});
    std::fill(projections.proj21.begin(), projections.proj21.end(), Projection{});
    std::fill(projections.proj22.begin(), projections.proj22.end(), Projection{});

    for (size_t i = 0; i < p1.count; i++)
    {
        const Vector2& axis = p1.worldNormals[i];
        Project(p1.worldVertices, axis, projections.proj11[i].min, projections.proj11[i].max);
        Project(p2.worldVertices, axis, projections.proj21[i].min, projections.proj21[i].max);
    }

    for (size_t i = 0; i < p2.count; i++)
    {
        const Vector2& axis = p2.worldNormals[i];
        Project(p1.worldVertices, axis, projections.proj12[i].min, projections.proj12[i].max);
        Project(p2.worldVertices, axis, projections.proj22[i].min, projections.proj22[i].max);
    }
}

void Render(const Projections& projections)
{
    // Normals are in the range [-1, 1] whereas vertices are in the range [0, SCREEN].
    // Any projections not in the +x +y quadrant won't be visible, hence the re-map!
    auto render = [](const Vector2& axis, float min, float max, float offset, Color color)
    {
        min = Remap(min, -SCREEN_WIDTH, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT * 0.5f);
        max = Remap(max, -SCREEN_WIDTH, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT * 0.5f);
        Vector2 start = axis * min;
        Vector2 end = axis * max;
        Vector2 tangent = CENTER + PerpendicularR(axis) * offset;
        DrawLineEx(start + tangent, end + tangent, 4.0f, color);
    };

    const float offset1 = 115.0f;
    const float offset2 = 125.0f;
    const Color color1 = SKYBLUE;
    const Color color2 = PINK;

    const Polygon& p1 = *projections.polygon1;
    const Polygon& p2 = *projections.polygon2;

    for (size_t i = 0; i < p1.count; i++)
    {
        const Vector2& axis = p1.worldNormals[i];
        render(axis, projections.proj11[i].min, projections.proj11[i].max, offset1, color1);
        render(axis, projections.proj21[i].min, projections.proj21[i].max, offset2, color2);
    }

    for (size_t i = 0; i < p2.count; i++)
    {
        const Vector2& axis = p2.worldNormals[i];
        render(axis, projections.proj12[i].min, projections.proj12[i].max, offset2, color2);
        render(axis, projections.proj22[i].min, projections.proj22[i].max, offset1, color1);
    }
}

void RenderAxes(const Projections& projections)
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

    for (const Vector2& axis : projections.polygon1->worldNormals)
        renderAxis(axis, DARKBLUE);

    for (const Vector2& axis : projections.polygon2->worldNormals)
        renderAxis(axis, DARKPURPLE);
}
/*
 * Visualization End *
 */

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);

    Polygon polygon1;
    Create(polygon1, 
    {
        { 0.0f, -0.66f },
        { 0.5f, 0.33f },
        { -0.5f, 0.33f }
    });

    Polygon polygon2;
    Create(polygon2,
    {
        { 0.0f, -0.66f },
        { 0.5f, 0.33f },
        { -0.5f, 0.33f }
    });

    // Visualization only. Otherwise no need to store projections
    Projections projections;
    Create(projections, polygon1, polygon2);

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
        // Time-based movement
        float dt = GetFrameTime();
        float translationDelta = translationSpeed * dt;
        float rotationDelta = rotationSpeed * dt;
        float scaleDelta = scaleSpeed * dt;

        // Decrease / increase scale
        if (IsKeyDown(KEY_LEFT_SHIFT))
            t1.scale -= scaleDelta;
        if (IsKeyDown(KEY_SPACE))
            t1.scale += scaleDelta;

        // Rotate counter-clockwise / clockwise
        if (IsKeyDown(KEY_Q))
            t1.rotation -= rotationDelta;
        if (IsKeyDown(KEY_E))
            t1.rotation += rotationDelta;

        // Move forwards / backwards
        Vector2 forward = Direction(t1.rotation);
        if (IsKeyDown(KEY_W))
            t1.translation = t1.translation + forward * translationDelta;
        if (IsKeyDown(KEY_S))
            t1.translation = t1.translation - forward * translationDelta;

        // Move left / right
        Vector2 right = PerpendicularR(forward);
        if (IsKeyDown(KEY_A))
            t1.translation = t1.translation - right * translationDelta;
        if (IsKeyDown(KEY_D))
            t1.translation = t1.translation + right * translationDelta;

        // Translate polygon2 up/down/left/right
        if (IsKeyDown(KEY_UP))
            t2.translation.y -= translationDelta;
        if (IsKeyDown(KEY_DOWN))
            t2.translation.y += translationDelta;
        if (IsKeyDown(KEY_LEFT))
            t2.translation.x -= translationDelta;
        if (IsKeyDown(KEY_RIGHT))
            t2.translation.x += translationDelta;

        // Test if polygons are colliding and render them accordingly!
        Update(polygon1);
        Update(polygon2);
        Update(projections);

        // Don't resolve so you can see how the overlap changes with position!
        //Vector2 mtv{};
        //CheckCollisionPolygons(polygon1, polygon2, &mtv);
        //t1.translation = t1.translation + mtv;
        //Update(polygon1);
        Color color = CheckCollisionPolygons(polygon1, polygon2) ? RED : GREEN;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        Render(polygon1, color, 5.0f);
        Render(polygon2, color, 5.0f);
        Render(projections);
        RenderAxes(projections);

        // Render instructions
        DrawLineEx(t1.translation, t1.translation + forward * t1.scale, 5.0f, GRAY);
        DrawText("SPACE / LSHIFT to scale up/down", 10, 10, 20, RED);
        DrawText("W / S to move forwards/backwards", 10, 30, 20, ORANGE);
        DrawText("D / A to move right/left", 10, 50, 20, BLUE);
        DrawText("E / Q to rotate right/left", 10, 70, 20, PURPLE);
        DrawText("Polygons will turn red when overlapping!", 10, 90, 20, color);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
