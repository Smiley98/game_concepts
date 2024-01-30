#include "rlImGui.h"
#include "Math.h"

constexpr int SCREEN_SIZE = 900;
constexpr int CELL_SIZE = 30;
constexpr int GRID_SIZE = SCREEN_SIZE / CELL_SIZE;  // 30

void DrawCell(int row, int col, Color color)
{
    DrawRectangle(col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE, color);
}

int main(void)
{
    InitWindow(SCREEN_SIZE, SCREEN_SIZE, "Game");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        Vector2 mouse = GetMousePosition();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawCell(4, 4, RED);
        for (int row = 0; row < GRID_SIZE; row++)
        {
            for (int col = 0; col < GRID_SIZE; col++)
            {
                Color color;
                color.r = Random(0.0f, 255.0f);
                color.g = Random(0.0f, 255.0f);
                color.b = Random(0.0f, 255.0f);
                color.a = 255;
                DrawCell(col, row, color);
            }
        }

        rlImGuiBegin();
        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}