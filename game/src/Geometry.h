#pragma once
#include "raylib.h"
#include <vector>
#include <fstream>
#include <algorithm>

struct Circle
{
    Vector2 position{};
    float radius = 0.0f;
};

using Points = std::vector<Vector2>;
using Obstacles = std::vector<Circle>;

inline Rectangle From(Circle circle)
{
    return {
        circle.position.x - circle.radius, circle.position.y - circle.radius,
        circle.radius * 2.0f, circle.radius * 2.0f
    };
}

inline Circle From(Rectangle rectangle)
{
    Vector2 center = { rectangle.x + rectangle.width * 0.5f, rectangle.y + rectangle.height * 0.5f };
    return { center, std::max(rectangle.width, rectangle.height) * 0.5f };
}

inline void SaveObstacles(const Obstacles& obstacles, const char* path = "../game/assets/data/obstacles.txt")
{
    std::ofstream file(path, std::ios::out | std::ios::trunc);
    for (const Circle& obstacle : obstacles)
        file << obstacle.position.x << " " << obstacle.position.y << " " << obstacle.radius << std::endl;
    file.close();
}

inline Obstacles LoadObstacles(const char* path = "../game/assets/data/obstacles.txt")
{
    Obstacles obstacles;
    std::ifstream file(path);
    while (!file.eof())
    {
        Circle obstacle;
        file >> obstacle.position.x >> obstacle.position.y >> obstacle.radius;
        obstacles.push_back(std::move(obstacle));
    }
    file.close();
    return obstacles;
}

inline void SavePoints(const Points& points, const char* path = "../game/assets/data/points.txt")
{
    std::ofstream file(path, std::ios::out | std::ios::trunc);
    for (const Vector2& point : points)
        file << point.x << " " << point.y << std::endl;
    file.close();
}

inline Points LoadPoints(const char* path = "../game/assets/data/points.txt")
{
    Points points;
    std::ifstream file(path);
    while (!file.eof())
    {
        Vector2 point;
        file >> point.x >> point.y;
        points.push_back(std::move(point));
    }
    file.close();
    return points;
}
