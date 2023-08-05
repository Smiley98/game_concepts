#include "rlImGui.h"
#include "Math.h"
#include <cstdio>
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Capsules can be thought of as two circles with a rectangle in-between them
struct Capsule
{
    Vector2 position{};
    Vector2 direction{ 1.0f, 0.0f };
    float radius;
    float halfLength;
    // Distance between position (center) and top/bot "circle"
};

// MTV (minimum translation vector) resolves circle from point (only written to on-collision)
bool PointCircle(Vector2 point, Vector2 circlePosition, float circleRadius, Vector2* mtv = nullptr)
{
    bool collision = DistanceSqr(point, circlePosition) <= circleRadius * circleRadius;
    if (collision && mtv != nullptr)
    {
        // Compare distance from projection to center vs radius to determine penetration depth,
        // then translate by penetration depth in the direction from circle to projection!
        Vector2 resolutionDirection = Normalize(circlePosition - point);
        float resolutionLength = circleRadius - Distance(point, circlePosition);
        *mtv = resolutionDirection * resolutionLength;
    }
    return collision;
}

// Project capsule onto line and line onto capsule. Resolve as point-circle collision.
bool LineCapsule(Vector2 lineStart, Vector2 lineEnd, Capsule capsule, Vector2* mtv = nullptr)
{
    Vector2 top = capsule.position + capsule.direction * capsule.halfLength;
    Vector2 bot = capsule.position - capsule.direction * capsule.halfLength;
    Vector2 projTop = ProjectPointLine(lineStart, lineEnd, top);
    Vector2 projBot = ProjectPointLine(lineStart, lineEnd, bot);
    Vector2 projCapsuleLine = DistanceSqr(top, projTop) < DistanceSqr(bot, projBot) ? projTop : projBot;
    Vector2 projLineCapsule = ProjectPointLine(top, bot, projCapsuleLine);
    return PointCircle(projCapsuleLine, projLineCapsule, capsule.radius, mtv);
}

void DrawCapsule(Capsule capsule, Color color)
{
    Vector2 top = capsule.position + capsule.direction * capsule.halfLength;
    Vector2 bot = capsule.position - capsule.direction * capsule.halfLength;
    DrawCircleV(top, capsule.radius, color);
    DrawCircleV(bot, capsule.radius, color);

    // Half-Length is along x because objects have the identity direction of [1, 0]
    Rectangle rec{ capsule.position.x, capsule.position.y,
        capsule.halfLength * 2.0f, capsule.radius * 2.0f };
    DrawRectanglePro(rec, { capsule.halfLength, capsule.radius },
        Angle(capsule.direction) * RAD2DEG, color);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);

    Vector2 lineCenter{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    Vector2 lineDirection{ 1.0f, 0.0f };
    float lineLength = 500.0f;

    Capsule capsule;
    capsule.halfLength = 50.0f;
    capsule.radius = 20.0f;

    HideCursor();
    while (!WindowShouldClose())
    {
        float rotation = 250.0f * DEG2RAD * GetFrameTime();
        capsule.position = GetMousePosition();
        if (IsKeyDown(KEY_E))
            lineDirection = Rotate(lineDirection, rotation);
        if (IsKeyDown(KEY_Q))
            lineDirection = Rotate(lineDirection, -rotation);
        if (IsKeyDown(KEY_D))
            capsule.direction = Rotate(capsule.direction, rotation);
        if (IsKeyDown(KEY_A))
            capsule.direction = Rotate(capsule.direction, -rotation);

        Vector2 lineStart = lineCenter + lineDirection * lineLength * 0.5f;
        Vector2 lineEnd = lineCenter + lineDirection * lineLength * -0.5f;
        Vector2 mtv;

        // Resolve line from capsule
        if (LineCapsule(lineStart, lineEnd, capsule, &mtv))
            lineCenter = lineCenter - mtv;
        
        // Determine colour after collision-resolution
        Color color = LineCapsule(lineStart, lineEnd, capsule) ? RED : GREEN;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawLineEx(lineStart, lineEnd, 5.0f, color);
        DrawCapsule(capsule, color);

        // Render projection math
        Vector2 top = capsule.position + capsule.direction * capsule.halfLength;
        Vector2 bot = capsule.position - capsule.direction * capsule.halfLength;
        Vector2 projTop = ProjectPointLine(lineStart, lineEnd, top);
        Vector2 projBot = ProjectPointLine(lineStart, lineEnd, bot);
        Vector2 projCapsuleLine = DistanceSqr(top, projTop) < DistanceSqr(bot, projBot) ? projTop : projBot;
        Vector2 projLineCapsule = ProjectPointLine(top, bot, projCapsuleLine);
        DrawCircleV(projCapsuleLine, 5.0f, BLUE);
        DrawCircleV(projLineCapsule, 5.0f, PURPLE);

        DrawText("Hold A & D to rotate the capsule clockwise/counter-clockwise.", 10, 10, 20, BLUE);
        DrawText("Hold E & Q to rotate the line clockwise/counter-clockwise.", 10, 30, 20, PURPLE);
        DrawText("Projection of capsule onto line in blue", 10, 50, 20, BLUE);
        DrawText("Projection of line onto capsule in purple", 10, 70, 20, PURPLE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
