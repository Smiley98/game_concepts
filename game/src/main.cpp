#include "rlImGui.h"
#include "Math.h"
#include <fstream>
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// Things like primitive data or structures containing only primitive data like this one play nice with binary files.
// Things that don't play nice with binary files:
// Pointers
// Textures
// Sound
// Strings
// (Literally anything that is not a collection of primitive data)
// 
// We can solve this by creating 2 different structures -- the "game" version and the "file" version (ie Circle is used in-game but its converted to and from IOCircle)
// We essentially write do this conversion process every time we use text files cause we need to write position, velocity, acceleration etc values.
// Binary automatically dumps to and from memory, so binary is still significantly more efficient. Its just we're making an algorithm to convert from binary to simpler binary instead of text to binary/

struct Circle
{
    Vector2 position;
    float radius;
    Texture* image; // Cannot save/load pointers in binary
};

enum ImageType
{

};

struct IOCircle
{
    Vector2 position;
    float radius;
    ImageType type; // Can save/load enum in binary, so just load an image in-game based on this type when loading the Circle into the game!
};

using namespace std;
int main(void)
{
    Circle circle;
    circle.position = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    circle.radius = 100.0f;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        Vector2 mouse = GetMousePosition();

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircleV(circle.position, circle.radius, RED);

        rlImGuiBegin();

        if (ImGui::Button("Save"))
        {
            circle.position = mouse;

            // If we don't care about revision history when saving, we should truncate (erase) the data so everything is up to date.
            // (Otherwise we've saved multiple circles to the file and we're reading whichever one is located at the file getter).
            ofstream file("Binary101.bin", ios::out | ios::binary | ios::trunc);
            file.write(reinterpret_cast<char*>(&circle), sizeof(Circle));
            file.close();
        }
        if (ImGui::Button("Load"))
        {
            ifstream file("Binary101.bin", ios::in | ios::binary);
            file.read(reinterpret_cast<char*>(&circle), sizeof(Circle));
            file.close();
        }
        if (ImGui::Button("Reset"))
        {
            circle.position = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
        }

        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}