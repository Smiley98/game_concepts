#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

enum Input
{
    KEYBOARD,
    MOUSE
};

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);

    Input input = KEYBOARD;
    const Vector2 center{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    const float radius = 250.0f;

    // Initial angle of 0 degrees ([1, 0] = horizontal = 0 degrees)
    Vector2 direction{ 1.0f, 0.0f };
    const Vector2 identity{ 1.0f, 0.0f };
    
    while (!WindowShouldClose())
    {
        if (input == KEYBOARD)
        {
            const float dt = GetFrameTime();

            // Rotate 100 degrees per second
            const float rotationSpeed = 100.0f * DEG2RAD;

            // Convert from units per second to units per frame
            const float rotation = rotationSpeed * dt;

            // Rotate direction either left or right
            if (IsKeyDown(KEY_Q))
                direction = Rotate(direction, -rotation);
            else if (IsKeyDown(KEY_E))
                direction = Rotate(direction, rotation);

            if (IsKeyPressed(KEY_SPACE))
                input = MOUSE;
        }
        else if (input == MOUSE)
        {
            // Calculate direction from screen center to mouse cursor
            direction = Normalize(GetMousePosition() - center);

            if (IsKeyPressed(KEY_SPACE))
                input = KEYBOARD;
        }

        float dot = Dot(identity, direction);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        const Vector2 top = center + Vector2{ 0.0f, -radius };
        const Vector2 bot = center + Vector2{ 0.0f, radius };
        const Vector2 left = center + Vector2{ -radius, 0.0f };
        const Vector2 right = center + Vector2{ radius, 0.0f };
        DrawCircleV(center, radius, RED);
        DrawLineEx(top, bot, 5.0f, BLACK);
        DrawLineEx(left, right, 5.0f, BLACK);
        DrawLineEx(center, center + direction * radius, 10.0f, GOLD);
        DrawLineEx(center, center + identity * radius, 5.0f, GREEN);

        const int fontSize = 20;
        const char* text0 = "0";
        const char* text90 = "90";
        const char* text180 = "180";
        const char* text270 = "270";
        const int size0 = MeasureText(text0, fontSize);
        const int size90 = MeasureText(text90, fontSize);
        const int size180 = MeasureText(text180, fontSize);
        const int size270 = MeasureText(text270, fontSize);
        DrawText(text0, right.x + size0, right.y - fontSize / 2, fontSize, DARKGRAY);
        DrawText(text90, bot.x - size90 / 2, bot.y + fontSize / 2, fontSize, DARKGRAY);
        DrawText(text180, left.x - (size180 + fontSize / 2), left.y - fontSize / 2, fontSize, DARKGRAY);
        DrawText(text270, top.x - size270 / 2, top.y - (3 * fontSize / 2), fontSize, DARKGRAY);

        const float angle180 = Angle(direction) * RAD2DEG;      //[-180, 180]
        const float angle360 = fmodf(angle180 + 360.0f, 360.0f);//[0, 360]
        DrawText(TextFormat("Direction: [%f, %f]", direction.x, direction.y), 10, 10, fontSize, RED);
        DrawText(TextFormat("Signed Angle: %f", angle180), 10, 10 + fontSize, fontSize, BLUE);
        DrawText(TextFormat("Unsigned Angle: %f", angle360), 10, 10 + 2 * fontSize, fontSize, PURPLE);
        DrawText(TextFormat("Dot Product: %f", dot), 10, 10 + 3 * fontSize, fontSize, GREEN);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
