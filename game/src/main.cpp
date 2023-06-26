#include "rlImGui.h"
#include "Math.h"
#include <vector>
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Catmull polynomial expansion
Vector2 Catmull(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, float t)
{
    // f(t) = 0.5(t^3, t^2, t, 1)MP
    // M =
    // [ -1,  3, -3,  1]
    // [  2, -5,  4, -1]
    // [ -1,  0,  1,  0]
    // [  0,  2,  0,  0]

    // MP = constraint matrix * points
    Vector2 r0 = p0 * -1.0f + p1 * 3.0f + p2 * -3.0f + p3;
    Vector2 r1 = p0 * 2.0f + p1 * -5.0f + p2 * 4.0f - p3;
    Vector2 r2 = p0 * -1.0f + p2;
    Vector2 r3 = p1 * 2.0f;

    // U = au^3 + bu^2 + cu + d (cubic polynomial * t)
    Vector2 p = (r3 + (r2 * t) + (r1 * t * t) + (r0 * t * t * t)) * 0.5f;
    return p;
}

// Render the curve as a series of lines along a catmull spline
void DrawLineCatmull(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3,
    Color color, float thick = 1.0f, bool dotted = false, size_t lines = 16)
{
    float t = 0.0f;
    const float step = 1.0f / (float)lines;
    for (size_t i = 0; i < lines; i++)
    {
        Vector2 start = Catmull(p0, p1, p2, p3, t);
        Vector2 end = Catmull(p0, p1, p2, p3, t + step);
        t += step;
        if (dotted && i % 2 == 0) continue;
        DrawLineEx(start, end, thick, color);
    }
}

// Determine control & end points along spline given an index
void Points(const std::vector<Vector2>& points, int index,
    Vector2& p0, Vector2& p1, Vector2& p2, Vector2& p3)
{
    const int n = points.size();
    p0 = points[(index - 1 + n) % n];
    p1 = points[index % n];
    p2 = points[(index + 1) % n];
    p3 = points[(index + 2) % n];
}

// Convenience overload for ImGui
void Points(std::vector<Vector2>& points, int index,
    Vector2*& p0, Vector2*& p1, Vector2*& p2, Vector2*& p3)
{
    const int n = points.size();
    p0 = &points[(index - 1 + n) % n];
    p1 = &points[index % n];
    p2 = &points[(index + 1) % n];
    p3 = &points[(index + 2) % n];
}

struct SamplePoint
{
    float t;    // Percentage along interval
    float d;    // Distance along interval
};

using SpeedTable = std::vector<std::vector<SamplePoint>>;

struct ControlledSpline
{
    std::vector<Vector2> points;
    size_t point = 0;   // Outer index -- determines p0, p1, p2 p3 using points
    size_t sample = 0;  // Inner index -- maps distance to interpolation using speedTable
    float distance = 0.0f;
    SpeedTable speedTable;
};

struct UncontrolledSpline
{
    std::vector<Vector2> points;
    size_t point = 0;
    float elapsed = 0.0f;
    float duration = 0.0f;
};

// Calculate arc length of each interval along the curve to map distance to interpolation
SpeedTable CreateSpeedTable(const std::vector<Vector2>& points, size_t samplesPerPoint = 16)
{
    SpeedTable speedTable;
    for (size_t i = 0; i < points.size(); i++)
    {
        std::vector<SamplePoint> samples;
        samples.push_back({ 0.0f, 0.0f });

        float arcLength = 0.0f;
        float step = 1.0f / (float)samplesPerPoint;

        for (float t = step; t <= 1.0f; t += step)
        {
            Vector2 p0, p1, p2, p3;
            Points(points, i, p0, p1, p2, p3);

            Vector2 previous = Catmull(p0, p1, p2, p3, t - step);
            Vector2 current = Catmull(p0, p1, p2, p3, t);

            arcLength += Distance(previous, current);
            samples.push_back({ t, arcLength });
        }

        speedTable.push_back(samples);
    }
    return speedTable;
}

// Convert distance to interpolation by looking up the arc length of the current interval
float DistanceToInterpolation(const ControlledSpline& spline)
{
    const size_t sampleCount = spline.speedTable[0].size();
    SamplePoint current = spline.speedTable[spline.point][spline.sample];
    SamplePoint next = spline.speedTable[spline.point][(spline.sample + 1) % sampleCount];
    return Lerp(current.t, next.t, (spline.distance - current.d) / (next.d - current.d));
}

// Increment sample index and point index based on distance travelled along the spline
void Update(ControlledSpline& spline)
{
    const size_t sampleCount = spline.speedTable[0].size();
    while (spline.distance > spline.speedTable[spline.point][(spline.sample + 1) % sampleCount].d)
    {
        if (++spline.sample >= sampleCount)
        {
            ++spline.point %= spline.speedTable.size();
            spline.sample = 0;
            spline.distance = 0.0f;
        }
    }
}

// Increment point index and reset elapsed if greater than duration
void Update(UncontrolledSpline& spline)
{
    if (spline.elapsed >= spline.duration)
    {
        spline.elapsed = 0.0f;
        ++spline.point %= spline.points.size();
    }
}

struct Entity
{
    Vector2 position{};
    Vector2 direction{ 1.0f, 0.0f };
};

void Follow(Entity& entity, ControlledSpline& spline, float speed)
{
    Vector2 p0, p1, p2, p3;

    // Determine previous point
    Points(spline.points, spline.point, p0, p1, p2, p3);
    Vector2 previous = Catmull(p0, p1, p2, p3, DistanceToInterpolation(spline));

    // Update distance and indices
    spline.distance += speed;
    Update(spline);

    // Determine current point
    Points(spline.points, spline.point, p0, p1, p2, p3);
    Vector2 current = Catmull(p0, p1, p2, p3, DistanceToInterpolation(spline));

    entity.position = current;
    entity.direction = Normalize(current - previous);
}

void Follow(Entity& entity, UncontrolledSpline& spline, float dt)
{
    Vector2 p0, p1, p2, p3;

    // Determine previous point
    Points(spline.points, spline.point, p0, p1, p2, p3);
    float tPrevious = Clamp(spline.elapsed / spline.duration, 0.0f, 1.0f);
    Vector2 previous = Catmull(p0, p1, p2, p3, tPrevious);

    // Update timer and index
    spline.elapsed += dt;
    Update(spline);

    // Determine current point
    Points(spline.points, spline.point, p0, p1, p2, p3);
    float tCurrent = Clamp(spline.elapsed / spline.duration, 0.0f, 1.0f);
    Vector2 current = Catmull(p0, p1, p2, p3, tCurrent);

    entity.position = current;
    entity.direction = Normalize(current - previous);
}

int main(void)
{
    InitWindow(1280, 720, "Game");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    std::vector<Vector2> points
    {
        { SCREEN_WIDTH * 0.2f, SCREEN_HEIGHT * 0.75f },
        { SCREEN_WIDTH * 0.4f, SCREEN_HEIGHT * 0.25f },
        { SCREEN_WIDTH * 0.6f, SCREEN_HEIGHT * 0.25f },
        { SCREEN_WIDTH * 0.8f, SCREEN_HEIGHT * 0.75f },
    };

    ControlledSpline controlledSpline;
    controlledSpline.points = points;
    controlledSpline.speedTable = CreateSpeedTable(points);

    UncontrolledSpline uncontrolledSpline;
    uncontrolledSpline.points = points;
    uncontrolledSpline.duration = 1.0f;

    Entity controlled;
    Entity uncontrolled;

    float speed = 500;
    while (!WindowShouldClose())
    {
        // Update entities
        const float dt = GetFrameTime();
        Follow(controlled, controlledSpline, speed * dt);
        Follow(uncontrolled, uncontrolledSpline, dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Render curve
        for (size_t i = 0; i < points.size(); i++)
        {
            Vector2 p0, p1, p2, p3;
            Points(points, i, p0, p1, p2, p3);
            DrawLineCatmull(p0, p1, p2, p3, { 130, 130, 130, 64 }, 5.0f);
            DrawCircleV(points[i], 10.0f, DARKGRAY);
        }

        // Render entities
        DrawCircleV(controlled.position, 20.0f, BLUE);
        DrawCircleV(uncontrolled.position, 20.0f, PURPLE);
        DrawLineV(controlled.position, controlled.position + controlled.direction * 100.0f, DARKBLUE);
        DrawLineV(uncontrolled.position, uncontrolled.position + uncontrolled.direction * 100.0f, DARKPURPLE);
        
        // Render info
        DrawText("Controlled -- Blue", 10, 10, 20, BLUE);
        DrawText("Uncontrolled -- Purple", 10, 30, 20, PURPLE);

        // Speed table must be regenerated if points change. Add at your own risk!
        rlImGuiBegin();
        ImGui::SliderFloat("Speed", &speed, 0.0f, 1000.0f);
        ImGui::SliderFloat("Duration", &uncontrolledSpline.duration, 0.1f, 10.0f);
        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
