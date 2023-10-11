#include "rlImGui.h"
#include "Math.h"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

struct Circle
{
    Vector2 position;
    float radius;
};

struct Plane
{
	Vector2 position;
	Vector2 normal;
};

Vector2 Perpendicular(Vector2 v)
{
    return { -v.y, v.x };
}

bool CheckCollision(Circle circle, Plane plane)
{
	// Project vector from plane to circle onto plane normal to determine distance from plane
	float distance = Dot(circle.position - plane.position, plane.normal);

	// Point-circle collision from here -- simply compare distance to circle's radius!
	return distance <= circle.radius;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    rlImGuiSetup(true);
    SetTargetFPS(60);

    const Vector2 center{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };

    Plane plane;
	plane.position = center;
	plane.normal = { 1.0f, 0.0f };

    Circle circle;
	circle.position = center;
	circle.radius = 25.0f;

    float lineLength = 69420.0f;
    float planeAngle = 0.0f;

    float angularSpeed = 100.0f * DEG2RAD;
    while (!WindowShouldClose())
    {
        float angularDelta = angularSpeed * GetFrameTime();
        if (IsKeyDown(KEY_Q))
            planeAngle -= angularDelta;
        if (IsKeyDown(KEY_E))
            planeAngle += angularDelta;
        planeAngle += PI * 2.0f;
        planeAngle = fmodf(planeAngle, PI * 2.0f);
        
        float distance = Dot(circle.position - plane.position, plane.normal);
        Vector2 proj = plane.position + plane.normal * distance;

		Vector2 planeDirection = Direction(planeAngle);
        plane.normal = Perpendicular(planeDirection);
		circle.position = GetMousePosition();

		bool collision = CheckCollision(circle, plane);
		Color color = collision ? RED : GREEN;

        BeginDrawing();
        ClearBackground(RAYWHITE);

		DrawLineEx(center - planeDirection * lineLength, center + planeDirection * lineLength, 5.0f, color);
		DrawLineEx(center, center + plane.normal * lineLength, 5.0f, GRAY);
		DrawCircleV(circle.position, circle.radius, color);
        DrawCircleV(proj, circle.radius, ORANGE);

        DrawText("Move the circle with your mouse, rotate the plane with E/Q.", 10, 10, 20, BLUE);
        DrawText("The circle will turn red/green depending on which side of the plane its on.", 10, 30, 20, color);
        DrawText("The orange circle is the projection of the circle onto the plane's normal.", 10, 50, 20, ORANGE);
        DrawText(TextFormat("Plane Angle: %f", planeAngle * RAD2DEG), 10, 70, 20, GRAY);

        rlImGuiBegin();
        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
