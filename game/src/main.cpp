#include "rlImGui.h"
int main(void)
{
    InitWindow(1280, 720, "Game");
    SetTargetFPS(60);
    Rectangle;
    float shapeWidth = 80.0f;
    float shapeHeight = 45.0f;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawLineEx({ 20.0f, 20.0f }, { 20.0f + shapeWidth, 20.0f + shapeHeight}, 5.0f, RED);
        DrawRectangleLinesEx({ 120.0f, 20.0f, shapeWidth, shapeHeight }, 5.0f, ORANGE);
        DrawRectangle(220.0f, 20.0f, shapeWidth, shapeHeight, GOLD);
        DrawRectangleGradientH(320.0f, 20.0f, shapeWidth, shapeHeight, RED, ORANGE);
        DrawRectangleGradientV(420.0f, 20.0f, shapeWidth, shapeHeight, RED, ORANGE);
        DrawRectangleGradientEx({ 420.0f, 20.0f, shapeWidth, shapeHeight },
            RED, ORANGE, BLUE, PURPLE);

        DrawCircleLines(60.0f, 120.0f, 30.0f, RED);
        DrawCircle(160.0f, 120.0f, 30.0f, ORANGE);
        DrawCircleGradient(260.0f, 120.0f, 30.0f, RED, ORANGE);
        DrawCircleSector({ 360.0f, 120.0f }, 30.0f, 0.0f, PI * RAD2DEG, 5, GOLD);
        DrawCircleSectorLines({ 460.0f, 120.0f }, 30.0f, 0.0f, PI * RAD2DEG, 5, GOLD);

        DrawRingLines({ 60.0f, 200.0f }, 20.0f, 30.0f, 0.0f, PI * RAD2DEG, 5, RED);
        DrawRing({ 160.0f, 200.0f }, 20.0f, 30.0f, 0.0f, PI * RAD2DEG, 5, ORANGE);
        DrawEllipse(360.0f, 200.0f, 20.0f, 30.0f, GOLD);
        DrawEllipseLines(460.0f, 200.0f, 20.0f, 30.0f, GOLD);

        DrawTriangleLines({ 30.0f, 250.0f }, { 90.0f, 250.0f }, { 60.0f, 300.0f }, RED);
        DrawTriangle({ 160.0f, 250.0f }, { 130.0f, 300.0f }, { 190.0f, 300.0f }, ORANGE);
        DrawPoly({ 260.0f, 275.0f }, 5, 30.0f, 0.0f, GOLD);
        DrawPolyLines({ 360.0f, 275.0f }, 5, 30.0f, 0.0f, GREEN);
        DrawPolyLines({ 460.0f, 275.0f }, 5, 30.0f, 35.0f, BLUE);

        // Uncomment to color a 160 x 90 square with all-random pixels!
        //for (int x = 20; x < 180; x++)
        //{
        //    for (int y = 320; y < 410; y++)
        //    {
        //        Color color =
        //        {
        //            GetRandomValue(0, 255), // red
        //            GetRandomValue(0, 255), // green
        //            GetRandomValue(0, 255), // blue
        //            255                     // alpha (transparency, 255 = fully opaque)
        //        };
        //        DrawPixel(x, y, color);
        //    }
        //}
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
