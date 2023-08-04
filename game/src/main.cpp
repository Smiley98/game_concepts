#include "rlImGui.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);

    bool toggleColour = false;
    Rectangle screen{ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    Vector2 position{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    while (!WindowShouldClose())
    {
        // Use time-based instead of frame-based movement to remain framerate-agnostic!
        float speed = 500.0f * GetFrameTime();

        if (IsKeyPressed(KEY_SPACE))
            toggleColour = !toggleColour;

        if (IsKeyDown(KEY_W))
            position.y -= speed;

        if (IsKeyDown(KEY_S))
            position.y += speed;

        if (IsKeyDown(KEY_A))
            position.x -= speed;

        if (IsKeyDown(KEY_D))
            position.x += speed;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangleRec(screen, toggleColour ? GREEN : RED);
        DrawCircleV(position, 25.0f, BLUE);
        DrawText("Hold WASD to move the blue circle.", 10, 10, 20, DARKGRAY);
        DrawText("Press SPACE to change between a green and red background.", 10, 30, 20, DARKGRAY);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}