#include "rlImGui.h"
int main(void)
{
    InitWindow(1280, 720, "Game");
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Hello World!", 16, 9, 20, RED);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}