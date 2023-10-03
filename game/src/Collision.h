#pragma once
#include "Shapes.h"

// MTV resolves circle from point
bool CheckCollisionPointCircle(Vector2 point,
    Vector2 circlePosition, float circleRadius, Vector2* mtv = nullptr)
{
    bool collision = DistanceSqr(point, circlePosition) <= circleRadius * circleRadius;
    if (collision && mtv != nullptr)
    {
        Vector2 resolutionDirection = Normalize(circlePosition - point);
        float resolutionLength = circleRadius - Distance(point, circlePosition);
        *mtv = resolutionDirection * resolutionLength;
    }
    return collision;
}

// MTV resolves circle from line
bool CheckCollisionLineCircle(Vector2 lineStart, Vector2 lineEnd,
    Vector2 circlePosition, float circleRadius, Vector2* mtv = nullptr)
{
    Vector2 nearest = ProjectPointLine(lineStart, lineEnd, circlePosition);
    bool collision = DistanceSqr(nearest, circlePosition) <= circleRadius * circleRadius;
    if (collision && mtv != nullptr)
    {
        Vector2 resolutionDirection = Normalize(circlePosition - nearest);
        float resolutionLength = circleRadius - Distance(nearest, circlePosition);
        *mtv = resolutionDirection * resolutionLength;
    }
    return collision;
}

// Project capsule onto line and line onto capsule. Resolve as point-circle collision.
bool CheckCollisionLineCapsule(Vector2 lineStart, Vector2 lineEnd,
    Capsule capsule, Vector2* mtv = nullptr)
{
    Vector2 top = capsule.position + capsule.direction * capsule.halfLength;
    Vector2 bot = capsule.position - capsule.direction * capsule.halfLength; Vector2 projTop = ProjectPointLine(lineStart, lineEnd, top);
    Vector2 projBot = ProjectPointLine(lineStart, lineEnd, bot);
    Vector2 projCapsuleLine = DistanceSqr(top, projTop) < DistanceSqr(bot, projBot) ? projTop : projBot;
    Vector2 projLineCapsule = ProjectPointLine(top, bot, projCapsuleLine);
    return  CheckCollisionPointCircle(projCapsuleLine, projLineCapsule, capsule.radius, mtv);
}

bool CheckCollisionCircleCircle(Circle circle1, Circle circle2, Vector2* mtv = nullptr)
{
    Vector2 delta = circle2.position - circle1.position;
    float radiiSum = circle1.radius + circle2.radius;
    float centerDistance = Length(delta);
    bool collision = centerDistance <= radiiSum;
    if (collision && mtv != nullptr)
    {
        // Compute mtv (sum of radii - distance between centers)
        float mtvDistance = radiiSum - centerDistance;
        *mtv = Normalize(delta) * mtvDistance;
    }
    return collision;
}

void NearestCirclePoints(Capsule capsule1, Capsule capsule2, Vector2& nearest1, Vector2& nearest2)
{
    Vector2 top1 = capsule1.position + capsule1.direction * capsule1.halfLength;
    Vector2 top2 = capsule2.position + capsule2.direction * capsule2.halfLength;
    Vector2 bot1 = capsule1.position - capsule1.direction * capsule1.halfLength;
    Vector2 bot2 = capsule2.position - capsule2.direction * capsule2.halfLength;

    Vector2 lines[4]
    {
        top2 - top1,
        bot2 - top1,
        top2 - bot1,
        bot2 - bot1,
    };

    // Determine nearest points between capsules
    size_t min = 0;
    for (size_t i = 1; i < 4; i++)
    {
        if (LengthSqr(lines[i]) < LengthSqr(lines[min]))
            min = i;
    }

    // 1. Find whether 1's top or bot is closer to 2
    // 2. Use that to find the point along 2 closest to 1
    // 3. Use the point along 2 closest to 1 to find the point along 1 closest to 2
    nearest1 = min < 2 ? top1 : bot1;
    nearest2 = ProjectPointLine(bot2, top2, nearest1);
    nearest1 = ProjectPointLine(bot1, top1, nearest2);
}

bool CheckCollisionCapsuleCapsule(Capsule capsule1, Capsule capsule2, Vector2* mtv = nullptr)
{
    Vector2 nearest1, nearest2;
    NearestCirclePoints(capsule1, capsule2, nearest1, nearest2);
    return CheckCollisionCircleCircle({ nearest1, capsule1.radius }, { nearest2, capsule2.radius }, mtv);
}

// TODO -- complete this!
bool CheckCollision(const Entity& entity1, const Entity& entity2, Vector2* mtv)
{
    assert(entity1.shape.type == NONE || entity2.shape.type == NONE);
    const Point& point1 = entity1.shape.geom.point;
    const Point& point2 = entity2.shape.geom.point;
    const Circle& circle1 = entity1.shape.geom.circle;
    const Circle& circle2 = entity2.shape.geom.circle;
    const Capsule& capsule1 = entity1.shape.geom.capsule;
    const Capsule& capsule2 = entity2.shape.geom.capsule;
    const Polygon& polygon1 = entity1.shape.geom.polygon;
    const Polygon& polygon2 = entity2.shape.geom.polygon;

    switch (entity1.shape.type)
    {
    case POINT:
        switch (entity2.shape.type)
        {
        case POINT:
            return CheckCollisionPointCircle(point1.position, point2.position, 1.0f, mtv);

        case CIRCLE:
            return CheckCollisionPointCircle(point1.position, circle2.position, circle2.radius, mtv);

        case CAPSULE:
            break;

        case POLYGON:
            break;
        }
        break;

    case CIRCLE:
        switch (entity2.shape.type)
        {
        case POINT:
            break;

        case CIRCLE:
            break;

        case CAPSULE:
            break;

        case POLYGON:
            break;
        }
        break;

    case CAPSULE:
        switch (entity2.shape.type)
        {
        case POINT:
            break;

        case CIRCLE:
            break;

        case CAPSULE:
            break;

        case POLYGON:
            break;
        }
        break;

    case POLYGON:
        switch (entity2.shape.type)
        {
        case POINT:
            break;

        case CIRCLE:
            break;

        case CAPSULE:
            break;

        case POLYGON:
            break;
        }
        break;
    }

    return false;
}