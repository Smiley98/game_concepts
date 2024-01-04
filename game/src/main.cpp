#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Scalar overlap test
inline bool Overlaps(float min1, float max1, float min2, float max2)
{
    return !((max1 < min2) || (max2 < min1));
}

// mtv points from rect2 to rect1
inline bool RectRect(Vector2 rect1, Vector2 extents1, Vector2 rect2, Vector2 extents2, Vector2* mtv = nullptr)
{
    float xMin1 = rect1.x - extents1.x;
    float xMax1 = rect1.x + extents1.x;
    float yMin1 = rect1.y - extents1.y;
    float yMax1 = rect1.y + extents1.y;

    float xMin2 = rect2.x - extents2.x;
    float xMax2 = rect2.x + extents2.x;
    float yMin2 = rect2.y - extents2.y;
    float yMax2 = rect2.y + extents2.y;

    bool collision = Overlaps(xMin1, xMax1, xMin2, xMax2) && Overlaps(yMin1, yMax1, yMin2, yMax2);
    if (collision && mtv != nullptr)
    {
        // Overlapping area (rectangle)
        float xMin = fmaxf(xMin1, xMin2);
        float xMax = fminf(xMax1, xMax2);
        float yMin = fmaxf(yMin1, yMin2);
        float yMax = fminf(yMax1, yMax2);

        // Resolve along the shortest axis (x vs y)
        float x = xMax - xMin;
        float y = yMax - yMin;
        if (x < y)
        {
            *mtv = { x, 0.0f };
            if (rect2.x > xMin) mtv->x *= -1.0f;
        }
        else if (y < x)
        {
            *mtv = { 0.0f, y };
            if (rect2.y > yMin) mtv->y *= -1.0f;
        }
        else
        {
            *mtv = { x, y };
            if (rect2.x > xMin) mtv->x *= -1.0f;
            if (rect2.y > yMin) mtv->y *= -1.0f;
        }
    }
    return collision;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);

    Vector2 position { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    while (!WindowShouldClose())
    {
        const float w = 60.0f;
        const float h = 40.0f;
        const Vector2 mouse = GetMousePosition();
        Vector2 mtv{};
        Vector2 extents = { w * 0.5f, h * 0.5f };
        RectRect(position, extents, mouse, extents, &mtv);
        position = position + mtv;
        Rectangle rec1{ position.x, position.y, w, h };
        Rectangle rec2{ mouse.x, mouse.y, w, h };

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectanglePro(rec1, extents, 0.0f, GREEN);
        DrawRectanglePro(rec2, extents, 0.0f, GREEN);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
