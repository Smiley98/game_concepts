#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

Color Lerp(Color A, Color B, float t)
{
    // Remap 0 from 255 to 0 to 1 so we can interpolate
    Vector4 nA = { A.r / 255.0f, A.g / 255.0f, A.b / 255.0f, A.a / 255.0f };
    Vector4 nB = { B.r / 255.0f, B.g / 255.0f, B.b / 255.0f, B.a / 255.0f };
    Vector4 nC = Lerp(nA, nB, t);

    // Remap from 0 to 1 to 0 to 255 to convert interpolated value back to a color
    return {
        (unsigned char)(nC.x * 255),
        (unsigned char)(nC.y * 255),
        (unsigned char)(nC.z * 255),
        (unsigned char)(nC.w * 255)
    };
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);

    const Vector2 position1{ SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.25f };
    const Vector2 position2{ SCREEN_WIDTH * 0.75f, SCREEN_HEIGHT * 0.75f };
    const float rotation1 = 0.0f;
    const float rotation2 = 90.0f;
    const Color color1 = RED;
    const Color color2 = GREEN;
    const Vector2 size1 = Vector2{ 60.0f, 40.0f };
    const Vector2 size2 = size1 * 3.0f;

    Vector2 position = position1;
    float rotation = rotation1;
    Vector2 size = size1;
    Color color = color1;

    while (!WindowShouldClose())
    {
        const float t = cosf(GetTime()) * 0.5f + 0.5f;
        position = Lerp(position1, position2, t);
        rotation = Lerp(rotation1, rotation2, t);
        size = Lerp(size1, size2, t);
        color = Lerp(color1, color2, t);

        const Vector2 origin{ size.x * 0.5f, size.y * 0.5f };
        const Vector2 origin1{ size1.x * 0.5f, size1.y * 0.5f };
        const Vector2 origin2{ size2.x * 0.5f, size2.y * 0.5f };
        
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectanglePro({ position1.x, position1.y, size1.x, size1.y },
            origin1, rotation1, color1);

        DrawRectanglePro({ position2.x, position2.y, size2.x, size2.y },
            origin2, rotation2, color2);

        DrawRectanglePro({ position.x, position.y, size.x, size.y },
            origin, rotation, color);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}