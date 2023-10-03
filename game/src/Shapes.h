#pragma once
#include "Math.h"
#include <cassert>
#include <vector>

struct Point
{
    Vector2 position{};
};

struct Circle
{
    Vector2 position{};
    float radius = 0.0f;
};

struct Capsule
{
    Vector2 position{};
    Vector2 direction{ 1.0f, 0.0f };
    float radius = 0.0f;
    float halfLength = 0.0f;
};

using Points = std::vector<Vector2>;
struct Polygon
{
    // Number of vertices
    size_t count = 0;

    // Original vertices & original normals
    Points modelVertices;
    Points modelNormals;

    // Transformed vertices & transformed normals
    Points worldVertices;
    Points worldNormals;
};

enum ShapeType
{
    NONE,
    POINT,
    CIRCLE,
    CAPSULE,
    POLYGON
};

union ShapeGeometry
{
    ShapeGeometry() {}
    ~ShapeGeometry() {}

    Point point;
    Circle circle;
    Capsule capsule;
    Polygon polygon;
};

struct Shape
{
    ShapeGeometry geom;
    ShapeType type;
};

struct RigidBody2
{
    Vector2 vel{};
    float angularSpeed = 0.0f;
};

struct RigidTransform2
{
    Vector2 translation{};
    float rotation = 0.0f;
};

struct Entity
{
    RigidTransform2 transform;
    RigidBody2 rb;
    Shape shape;

    void Update(float dt, Vector2 acc)
    {
        rb.vel = rb.vel + acc * dt;
        transform.translation = transform.translation + rb.vel * dt + acc * (dt * dt) * 0.5f;
        transform.rotation = transform.rotation + rb.angularSpeed * dt;

        // Better than polymorphism xD
        assert(shape.type != NONE);
        switch (shape.type)
        {
        case POINT:
        case CIRCLE:
            shape.geom.circle.position = transform.translation;
            break;

        case CAPSULE:
            shape.geom.capsule.position = transform.translation;
            shape.geom.capsule.direction = Direction(transform.rotation);
            break;

        case POLYGON:
            Matrix translation = Translate(transform.translation.x, transform.translation.y, 0.0f);
            Matrix rotation = RotateZ(transform.rotation);
            Matrix transformation = rotation * translation;

            Polygon& polygon = reinterpret_cast<Polygon&>(shape.geom.polygon);
            for (size_t i = 0; i < polygon.count; i++)
            {
                polygon.worldVertices[i] = Multiply(polygon.modelVertices[i], transformation);
                polygon.worldNormals[i] = Multiply(polygon.modelNormals[i], rotation);
            }
            break;
        }
    }
};

bool CheckCollision(const Entity& entity1, const Entity& entity2, Vector2* mtv = nullptr);

void DrawCircle(Circle circle, Color color)
{
    DrawCircleLines(circle.position.x, circle.position.y, circle.radius, color);
}

void DrawCapsule(Capsule capsule, Color color)
{
    Vector2 top = capsule.position + capsule.direction * capsule.halfLength;
    Vector2 bot = capsule.position - capsule.direction * capsule.halfLength;
    Vector2 perp = Vector2{ capsule.direction.y, -capsule.direction.x } * capsule.radius;
    
    float rotation = Angle(capsule.direction) * RAD2DEG;
    DrawCircleSectorLines(top, capsule.radius, rotation + -90.0f, rotation + 90.0f, 5, color);
    DrawCircleSectorLines(bot, capsule.radius, rotation + 270.0f, rotation + 90.0f, 5, color);
    DrawLineV(top - perp, bot - perp, color);
    DrawLineV(top + perp, bot + perp, color);

    // Filled:
    //DrawCircleV(top, capsule.radius, color);
    //DrawCircleV(bot, capsule.radius, color);
    //Rectangle rec{ capsule.position.x, capsule.position.y,
    //    capsule.halfLength * 2.0f, capsule.radius * 2.0f };
    //DrawRectanglePro(rec, { capsule.halfLength, capsule.radius },
    //    Angle(capsule.direction) * RAD2DEG, color);
}

void DrawPolygon(const Polygon& polygon, Color color)
{
    for (size_t i = 0; i < polygon.count; i++)
    {
        Vector2 p0 = polygon.worldVertices[i];
        Vector2 p1 = polygon.worldVertices[(i + 1) % polygon.count];
        DrawLineV(p0, p1, color);
    }
}