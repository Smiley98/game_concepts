#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

struct Circle
{
    Vector2 position{};
    float radius;
};

// Capsules can be thought of as two circles with a rectangle in-between them
// Capsule-Capsule collision works by querying the two nearest circles between capsules
struct Capsule
{
    Vector2 position{};
    Vector2 direction{ 1.0f, 0.0f };
    float radius;
    float halfLength;
    // Distance between position (center) and top/bot "circle"
};

void CapsulePoints(Capsule capsule, Vector2& top, Vector2& bot)
{
    top = capsule.position + capsule.direction * capsule.halfLength;
    bot = capsule.position - capsule.direction * capsule.halfLength;
}

void NearestCirclePoints(Capsule capsule1, Capsule capsule2, Vector2& nearest1, Vector2& nearest2)
{
    Vector2 top1, top2, bot1, bot2;
    CapsulePoints(capsule1, top1, bot1);
    CapsulePoints(capsule2, top2, bot2);

    Vector2 lines[4]
    {
        top2 - top1,
        bot2 - top1,
        top2 - bot1,
        bot2 - bot1,
    };

    // Determine nearest points between capsules
    size_t min = 0;
    for (size_t i = 1; i < 4; i++)
    {
        if (LengthSqr(lines[i]) < LengthSqr(lines[min]))
            min = i;
    }

    // 1. Find whether 1's top or bot is closer to 2
    // 2. Use that to find the point along 2 closest to 1
    // 3. Use the point along 2 closest to 1 to find the point along 1 closest to 2
    nearest1 = min < 2 ? top1 : bot1;
    nearest2 = ProjectPointLine(bot2, top2, nearest1);
    nearest1 = ProjectPointLine(bot1, top1, nearest2);
}

bool CircleCircle(Circle circle1, Circle circle2, Vector2* mtv = nullptr)
{
    float distance = Distance(circle1.position, circle2.position);
    float radiiSum = circle1.radius + circle2.radius;
    bool collision = distance <= radiiSum;
    if (collision && mtv != nullptr)
    {
        *mtv = Normalize(circle1.position - circle2.position) * (radiiSum - distance);
    }
    return collision;
}

bool CircleCapsule(Circle circle, Capsule capsule, Vector2* mtv = nullptr)
{
    Vector2 top = capsule.position + capsule.direction * capsule.halfLength;
    Vector2 bot = capsule.position - capsule.direction * capsule.halfLength;
    Vector2 proj = ProjectPointLine(top, bot, circle.position);
    return CircleCircle(circle, { proj, capsule.radius }, mtv);
}

bool CapsuleCapsule(Capsule capsule1, Capsule capsule2, Vector2* mtv = nullptr)
{
    Vector2 nearest1, nearest2;
    NearestCirclePoints(capsule1, capsule2, nearest1, nearest2);
    return CircleCircle({ nearest1, capsule1.radius }, { nearest2, capsule2.radius }, mtv);
}

void DrawCapsule(Capsule capsule, Color color)
{
    Vector2 top, bot;
    CapsulePoints(capsule, top, bot);
    DrawCircleV(top, capsule.radius, color);
    DrawCircleV(bot, capsule.radius, color);

    // Half-Length is along x because objects have the identity direction of [1, 0]
    Rectangle rec{ capsule.position.x, capsule.position.y,
        capsule.halfLength * 2.0f, capsule.radius * 2.0f, };
    DrawRectanglePro(rec, { capsule.halfLength, capsule.radius, },
        Angle(capsule.direction) * RAD2DEG, color);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);

    float angularSpeed = 100.0f * DEG2RAD;
    float radius = 25.0f;
    float halfLength = 50.0f;

    Circle circle;
    circle.position = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.25f };
    circle.radius = radius;

    Capsule capsule1;
    capsule1.position = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.75f };
    capsule1.radius = radius;
    capsule1.halfLength = halfLength;

    Capsule capsule2;
    capsule2.direction = Direction(90.0f * DEG2RAD);
    capsule2.radius = radius;
    capsule2.halfLength = halfLength;

    while (!WindowShouldClose())
    {
        capsule2.position = GetMousePosition();
        const float dt = GetFrameTime();
        if (IsKeyDown(KEY_E))
            capsule1.direction = Rotate(capsule1.direction, angularSpeed * dt);
        if (IsKeyDown(KEY_Q))
            capsule1.direction = Rotate(capsule1.direction, -angularSpeed * dt);
        if (IsKeyDown(KEY_D))
            capsule2.direction = Rotate(capsule2.direction, angularSpeed * dt);
        if (IsKeyDown(KEY_A))
            capsule2.direction = Rotate(capsule2.direction, -angularSpeed * dt);

        // Circle vs projected circle
        Vector2 top, bot;
        CapsulePoints(capsule2, top, bot);
        Vector2 proj = ProjectPointLine(bot, top, circle.position);

        // Projected circle1 vs projected circle2
        Vector2 nearest1, nearest2;
        NearestCirclePoints(capsule1, capsule2, nearest1, nearest2);

        Vector2 mtv;
        Color circleColor, capsule1Color;
        circleColor = capsule1Color = GREEN;

        bool circleCapsuleCollision = CircleCapsule(circle, capsule2, &mtv);
        if (circleCapsuleCollision)
        {
            circle.position = circle.position + mtv;
            circleColor = RED;
        }

        bool capsuleCollision = CapsuleCapsule(capsule1, capsule2, &mtv);
        if (capsuleCollision)
        {
            capsule1.position = capsule1.position + mtv;
            capsule1Color = RED;
        }

        Color capsule2Color = circleCapsuleCollision || capsuleCollision ? RED : GREEN;
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(circle.position, circle.radius, circleColor);
        DrawCapsule(capsule1, capsule1Color);
        DrawCapsule(capsule2, capsule2Color);
        DrawCircleV(proj, circle.radius, SKYBLUE);
        DrawCircleV(nearest1, capsule1.radius, BLUE);
        DrawCircleV(nearest2, capsule2.radius, PURPLE);
        DrawText("The circle & capsule collide when the teal circle overlaps the circle", 10, 10, 20, GRAY);
        DrawText("The capsules collide when the blue & purple circles overlap", 10, 30, 20, GRAY);
        DrawText("Press D & A to rotate the cursor-controlled capsule", 10, 50, 20, PURPLE);
        DrawText("Press E & Q to rotate the stationary capsule", 10, 70, 20, BLUE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
