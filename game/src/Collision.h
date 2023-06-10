#pragma once
#include "raylib.h"
#include "Math.h"
#include "Geometry.h"
#include <cassert>

inline bool CheckCollisionPointCircle(Vector2 point, Circle circle)
{
    return DistanceSqr(circle.position, point) <= circle.radius * circle.radius;
}

inline bool CheckCollisionCircles(Circle circle1, Circle circle2)
{
    return DistanceSqr(circle1.position, circle2.position) <=
        powf(circle1.radius + circle2.radius, 2.0f);
}

// MTV (minimum translation vector) resolves circle 2 from circle 1
inline bool CheckCollisionCircles(Circle circle1, Circle circle2, Vector2& mtv)
{
    if (!CheckCollisionCircles(circle1, circle2)) return false;
    Vector2 AB = circle2.position - circle1.position;
    const float radiiSum = circle1.radius + circle2.radius;
    const float penetrationDepth = radiiSum - Length(AB);
    mtv = Normalize(AB) * penetrationDepth;
    return true;
}

inline bool CheckCollisionLineCircle(Vector2 lineStart, Vector2 lineEnd, Circle circle)
{
    Vector2 nearest = NearestPoint(lineStart, lineEnd, circle.position);
    return DistanceSqr(nearest, circle.position) <= circle.radius * circle.radius;
}

inline bool CheckCollisionLineCircle(Vector2 lineStart, Vector2 lineEnd, Circle circle, Vector2& poi)
{
    Vector2 dc = lineStart - circle.position;
    Vector2 dx = lineEnd - lineStart;
    float a = LengthSqr(dx);
    float b = Dot(dx, dc) * 2.0f;
    float c = LengthSqr(dc) - circle.radius * circle.radius;
    float det = b * b - 4.0f * a * c;

    if (a <= FLT_EPSILON || det < 0.0f) return false;

    det = sqrtf(det);
    float t1 = (-b - det) / (2.0f * a);
    float t2 = (-b + det) / (2.0f * a);

    Vector2 poiA{ lineStart + dx * t1 };
    Vector2 poiB{ lineStart + dx * t2 };

    // Line is infinite so we must restrit it between start and end via bounding rectangle
    float xMin = std::min(lineStart.x, lineEnd.x);
    float xMax = std::max(lineStart.x, lineEnd.x);
    float yMin = std::min(lineStart.y, lineEnd.y);
    float yMax = std::max(lineStart.y, lineEnd.y);
    Rectangle rec{ xMin, yMin, xMax - xMin, yMax - yMin };

    bool collisionA = CheckCollisionPointRec(poiA, rec);
    bool collisionB = CheckCollisionPointRec(poiB, rec);
    if (collisionA && collisionB)
    {
        poi = DistanceSqr(lineStart, poiA) < DistanceSqr(lineStart, poiB) ? poiA : poiB;
        return true;
    }
    if (collisionA)
    {
        poi = poiA;
        return true;
    }
    if (collisionB)
    {
        poi = poiB;
        return true;
    }
    return false;
}

inline Vector2 NearestPoint(const Vector2& point, const Points& points)
{
    assert(!points.empty());
    return *min_element(points.begin(), points.end(),
        [&point](const Vector2& a, const Vector2& b) -> bool
        {
            return DistanceSqr(a, point) < DistanceSqr(b, point);
        });
}

inline bool NearestIntersection(Vector2 lineStart, Vector2 lineEnd, const Obstacles& obstacles, Vector2& poi)
{
    Points intersections;
    intersections.reserve(obstacles.size());

    for (const Circle& obstacle : obstacles)
    {
        Vector2 poi;
        if (CheckCollisionLineCircle(lineStart, lineEnd, obstacle, poi))
            intersections.push_back(std::move(poi));
    }

    if (!intersections.empty())
        poi = NearestPoint(lineStart, intersections);
    return !intersections.empty();
}

inline bool IsPointVisible(Vector2 lineStart, Vector2 lineEnd, const Obstacles& obstacles)
{
    for (const Circle& obstacle : obstacles)
    {
        if (CheckCollisionLineCircle(lineStart, lineEnd, obstacle))
            return false;
    }
    return true;
}

inline bool IsCircleVisible(Vector2 lineStart, Vector2 lineEnd, Circle circle, const Obstacles& obstacles)
{
    Vector2 circlePoi;
    bool circleCollision = CheckCollisionLineCircle(lineStart, lineEnd, circle, circlePoi);
    if (!circleCollision) return false;

    Vector2 obstaclePoi;
    bool obstacleCollision = NearestIntersection(lineStart, lineEnd, obstacles, obstaclePoi);
    if (!obstacleCollision) return true;

    return DistanceSqr(circlePoi, lineStart) < DistanceSqr(obstaclePoi, lineStart);
}
