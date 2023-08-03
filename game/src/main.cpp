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
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
            toggleColour = !toggleColour;

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            position = GetMousePosition();

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangleRec(screen, toggleColour ? GREEN : RED);
        DrawCircleV(position, 25.0f, BLUE);
        DrawText("Hold down the left mouse button to drag around the blue circle.", 10, 10, 20, DARKGRAY);
        DrawText("Left-click to change between a green and red background.", 10, 30, 20, DARKGRAY);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}