#include "rlImGui.h"
#include "Math.h"
#include <array>

constexpr int SCREEN_SIZE = 900;
constexpr int CELL_SIZE = 30;
constexpr int GRID_SIZE = SCREEN_SIZE / CELL_SIZE;  // 30

using Cells = std::array<std::array<bool, GRID_SIZE>, GRID_SIZE>;

void DrawCell(int col, int row, Color color)
{
    DrawRectangle(col * CELL_SIZE, row * CELL_SIZE, CELL_SIZE, CELL_SIZE, color);
}

int Neighbours(int col, int row, Cells cells)
{
    int neighbours = 0;

    int left = col - 1;
    int right = col + 1;
    int top = row - 1;
    int bot = row + 1;

    bool canLeft = left >= 0;
    bool canRight = right < GRID_SIZE;
    bool canTop = top >= 0;
    bool canBot = bot < GRID_SIZE;

    if (canLeft) neighbours += cells[left][row] ? 1 : 0;
    if (canRight) neighbours += cells[right][row] ? 1 : 0;
    if (canTop) neighbours += cells[col][top] ? 1 : 0;
    if (canBot) neighbours += cells[col][bot] ? 1 : 0;

    if (canLeft && canTop) neighbours += cells[left][top] ? 1 : 0;
    if (canLeft && canBot) neighbours += cells[left][bot] ? 1 : 0;
    if (canRight && canTop) neighbours += cells[right][top] ? 1 : 0;
    if (canRight && canBot) neighbours += cells[right][bot] ? 1 : 0;

    return neighbours;
}

Color Palette(int col, int row);

int main(void)
{
    InitWindow(SCREEN_SIZE, SCREEN_SIZE, "Game");
    SetTargetFPS(60);

    Cells cells;
    memset(cells.data(), 0, GRID_SIZE * GRID_SIZE);

    cells[13][14] = true;
    cells[13][15] = true;
    cells[13][16] = true;

    // 3x3 alive cells excluding centre cell
    cells[14][14] = true;
    cells[14][15] = false;
    cells[14][16] = true;

    cells[15][14] = true;
    cells[15][15] = true;
    cells[15][16] = true;

    while (!WindowShouldClose())
    {
        for (int col = 0; col < GRID_SIZE; col++)
        {
            for (int row = 0; row < GRID_SIZE; row++)
            {
                int neighbours = Neighbours(col, row, cells);
                if (cells[col][row])
                {
                    // survives if 2-3 neighbours
                    cells[col][row] = neighbours == 2 || neighbours == 3;
                }
                else
                {
                    // revives if 3 neighbours
                    cells[col][row] = neighbours == 3;
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (int col = 0; col < GRID_SIZE; col++)
        {
            for (int row = 0; row < GRID_SIZE; row++)
            {
                bool alive = cells[col][row];
                Color color = alive ? Palette(col, row) : BLACK;
                DrawCell(col, row, color);
            }
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

Color Palette(int col, int row)
{
    constexpr int centre = GRID_SIZE / 2;
    int x = abs(col - centre);
    int y = abs(row - centre);
    int size = x + y;

    if (size < 3) return RED;
    if (size < 6) return ORANGE;
    if (size < 9) return YELLOW;
    if (size < 12) return GREEN;
    if (size < 15) return BLUE;
    if (size < 18) return PURPLE;
    if (size < 21) return WHITE;
}
