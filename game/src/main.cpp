#include "rlImGui.h"
#include "Math.h"
#include <vector>
constexpr float SCREEN_WIDTH = 1280.0f;
constexpr float SCREEN_HEIGHT = 720.0f;
constexpr size_t BRICK_ROWS = 8;
constexpr size_t BRICK_COLS = 8;
constexpr float BRICK_WIDTH = SCREEN_WIDTH / (BRICK_COLS + 2);
constexpr float BRICK_HEIGHT = SCREEN_HEIGHT / (BRICK_ROWS + 4);
constexpr float PLAYER_WIDTH = BRICK_WIDTH;
constexpr float PLAYER_HEIGHT = BRICK_HEIGHT;
constexpr float BALL_RADIUS = 25.0f;

struct Entity;
using OnCollision = void(*)(Entity& self, Entity& other);

enum Shape
{
    CIRCLE,
    PLANE,
    AABB
};

enum Tag
{
    PLAYER,
    BRICK,
    BALL,
    WALL,
    INVALID
};

struct Collider
{
    union
    {
        Vector2 normal{};
        Vector2 extents;
        float radius;
    };

    Shape shape = CIRCLE;
};

struct Entity
{
    Vector2 pos{};
    Vector2 vel{};
    Vector2 acc{};
    Vector2 force{};

    float gravityScale = 1.0f;
    float invMass = 1.0f;
    float restitution = 1.0f;

    Collider collider;
    OnCollision onCollision = nullptr;
    Tag tag = INVALID;
    bool disabled = false;
};

using Entities = std::vector<Entity>;

struct Manifold
{
    Entity* a = nullptr;
    Entity* b = nullptr;
    Vector2 mtv{};
};

// mtv points from plane to circle
inline bool CirclePlane(Vector2 circle, float radius, Vector2 plane, Vector2 normal, Vector2* mtv = nullptr)
{
    float distance = Dot(circle - plane, normal);
    bool collision = distance <= radius;
    if (collision && mtv != nullptr)
        *mtv = normal * (radius - distance);
    return collision;
}

// mtv points from rect to circle
inline bool CircleRect(Vector2 circle, float radius, Vector2 rect, Vector2 extents, Vector2* mtv = nullptr)
{
    Vector2 nearest = Clamp(circle, rect - extents, rect + extents);
    return CirclePlane(circle, radius, nearest, Normalize(circle - nearest), mtv);
}

inline bool HitTest(Vector2 pos1, Vector2 pos2, Collider col1, Collider col2, Vector2* mtv = nullptr)
{
    if (col1.shape == CIRCLE && col2.shape == PLANE)
        return CirclePlane(pos1, col1.radius, pos2, col2.normal, mtv);

    if (col1.shape == PLANE && col2.shape == CIRCLE)
        return CirclePlane(pos2, col2.radius, pos1, col1.normal, mtv);

    if (col1.shape == CIRCLE && col2.shape == AABB)
        return CircleRect(pos1, col1.radius, pos2, col2.extents, mtv);

    if (col1.shape == AABB && col2.shape == CIRCLE)
        return CircleRect(pos2, col2.radius, pos1, col1.extents, mtv);

    return false;
}

inline std::vector<Manifold> HitTest(const std::vector<Entity>& entities)
{
    std::vector<Manifold> collisions;
    for (size_t i = 0; i < entities.size(); i++)
    {
        for (size_t j = i + 1; j < entities.size(); j++)
        {
            Manifold manifold;
            const Entity& a = entities[i];
            const Entity& b = entities[j];
            if (a.disabled || b.disabled) continue;
            if (HitTest(a.pos, b.pos, a.collider, b.collider, &manifold.mtv))
            {
                // Ensure A is always dynamic and B is either static or dynamic
                if (!a.invMass > 0.0f && b.invMass > 0.0f)
                    manifold.a = (Entity*)&b, manifold.b = (Entity*)&a;
                else
                    manifold.a = (Entity*)&a, manifold.b = (Entity*)&b;

                collisions.push_back(manifold);
            }
        }
    }
    return collisions;
}

inline void ResolveVelocity(Manifold collision)
{
    // e = restitution, j = impulse, jt = tangent impulse (friciton), mu = friction coefficient
    Entity& a = *collision.a;
    Entity& b = *collision.b;

    // Exit if objects are separating or both have infinite masses
    Vector2 normal = Normalize(collision.mtv);
    Vector2 velBA = a.vel - b.vel;
    float t = Dot(velBA, normal);
    if (t > 0.0f) return;
    if ((a.invMass + b.invMass) <= FLT_MIN) return;

    // Restitution
    float e = fminf(a.restitution, b.restitution);
    float j = -(1.0f + e) * t / (a.invMass + b.invMass);
    a.vel = a.vel + normal * j * a.invMass;
    b.vel = b.vel - normal * j * b.invMass;
}

inline void ResolvePosition(Manifold collision)
{
    Entity& a = *collision.a;
    Entity& b = *collision.b;
    if (b.invMass > 0.0f)
    {
        a.pos = a.pos + collision.mtv * 0.5f;
        b.pos = b.pos - collision.mtv * 0.5f;
    }
    else
        a.pos = a.pos + collision.mtv;
}

void Step(float dt, Entities& entities)
{
    for (Entity& entity : entities)
    {
        entity.acc = entity.force * entity.invMass;
        entity.vel = entity.vel + entity.acc * dt;
        entity.pos = entity.pos + entity.vel * dt;
        entity.force = {};
    }

    std::vector<Manifold> collisions = HitTest(entities);
    for (const Manifold& collision : collisions)
    {
        ResolveVelocity(collision);
        ResolvePosition(collision);

        Entity& a = *collision.a;
        Entity& b = *collision.b;
        if (a.onCollision != nullptr)
            a.onCollision(a, b);
        if (b.onCollision != nullptr)
            b.onCollision(b, a);
    }
}

void Update(float dt, Entities& entities)
{
    static float prevTime, currTime, timestep = 1.0f / 50.0f;
    while (prevTime < currTime)
    {
        prevTime += timestep;
        Step(timestep, entities);
    }
    currTime += dt;
}

Entity CreateWall(Vector2 position, Vector2 normal)
{
    Entity plane;
    plane.collider.shape = PLANE;
    plane.collider.normal = normal;

    plane.pos = position;
    plane.gravityScale = 0.0f;
    plane.invMass = 0.0f;
    plane.restitution = 1.0f;

    plane.tag = WALL;
    return plane;
}

Entity CreateBrick(Vector2 position)
{
    Entity brick;
    brick.collider.shape = AABB;
    brick.collider.extents = { BRICK_WIDTH * 0.5f, BRICK_HEIGHT * 0.5f };

    brick.pos = position;
    brick.gravityScale = 0.0f;
    brick.invMass = 0.0f;
    brick.restitution = 1.0f;

    brick.tag = BRICK;
    return brick;
}

void BallCollisionHandler(Entity& ball, Entity& other);
Entity CreateBall(Vector2 position)
{
    Entity ball;
    ball.collider.shape = CIRCLE;
    ball.collider.radius = BALL_RADIUS;

    ball.pos = position;
    ball.vel = { 0.0f, -10.0f };
    ball.gravityScale = 0.0f;
    ball.invMass = 1.0f;
    ball.restitution = 1.0f;

    ball.onCollision = BallCollisionHandler;
    ball.tag = BALL;
    return ball;
}

int lives = 3;
int breaks = 0;
Entity *player, *ball1, *ball2;
Vector2 ball1Spawn = { BRICK_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
Vector2 ball2Spawn = { SCREEN_WIDTH - BRICK_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };

enum GameState
{
    PLAY,
    WIN,
    LOSS,
    COUNT
} gameState;

int main(void)
{
    Entities entities;
    entities.reserve(1024);

    entities.push_back({});
    player = &entities.front();
    player->collider.shape = AABB;
    player->collider.extents = { PLAYER_WIDTH * 0.5f, PLAYER_HEIGHT * 0.5f };

    player->pos = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT - PLAYER_HEIGHT * 0.5f };
    player->invMass = 0.0f;
    player->gravityScale = 0.0f;
    player->tag = PLAYER;

    entities.push_back(CreateWall({ 0.0f, 0.0f }, { 0.0f, -1.0f }));	    // top
    entities.push_back(CreateWall({ 0.0f, SCREEN_HEIGHT }, { 0.0f, 1.0f }));// bottom
    entities.push_back(CreateWall({ 0.0f, 0.0f }, { 1.0f, 0.0f }));	        // left
    entities.push_back(CreateWall({ SCREEN_WIDTH, 0.0f }, { -1.0f, 0.0f }));// right

    entities.push_back(CreateBall(ball1Spawn));
    ball1 = &entities.back();

    entities.push_back(CreateBall(ball2Spawn));
    ball2 = &entities.back();

    float x, y = SCREEN_HEIGHT - BRICK_HEIGHT * 3.5f;
    for (size_t i = 0; i < BRICK_ROWS; i++)
    {
        x = 0.0f + BRICK_WIDTH * 1.5f;
        for (size_t j = 0; j < BRICK_COLS; j++)
        {
            entities.push_back(CreateBrick({ x, y }));
            x += BRICK_WIDTH;
        }
        y -= BRICK_HEIGHT;
    }

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        float speed = 5.0f;
        Vector2 direction = {};
        if (IsKeyDown(KEY_A))
            direction.x -= 1.0f;
        if (IsKeyDown(KEY_D))
            direction.x += 1.0f;
        player->vel = direction * speed;
        Update(GetFrameTime(), entities);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (const Entity& entity : entities)
        {
            float w = entity.collider.extents.x * 2.0f;
            float h = entity.collider.extents.y * 2.0f;
            float x = entity.pos.x - w * 0.5f;
            float y = entity.pos.y - h * 0.5f;
            if (entity.disabled) continue;
            switch (entity.tag)
            {
            case BALL:
                DrawCircleV(entity.pos, entity.collider.radius, GRAY);
                break;
        
            case BRICK:
                DrawRectangle(x, y, w, h, BLUE);
                DrawRectangleLines(x, y, w, h, PURPLE);
                break;
        
            case PLAYER:
                DrawRectangle(x, y, w, h, GREEN);
                DrawRectangleLines(x, y, w, h, RED);
                break;
            }
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void OnReset()
{
    ball1->pos = ball1Spawn;
    ball2->pos = ball2Spawn;
    ball1->vel = ball2->vel = { 0.0f, -10.0f };
    ball1->disabled = ball2->disabled = false;
    lives--;
    if (lives < 0)
        gameState = LOSS;
}

void BallCollisionHandler(Entity& ball, Entity& other)
{
    if (other.tag == BRICK)
    {
        other.disabled = true;
        breaks++;
        if (breaks >= BRICK_ROWS * BRICK_COLS)
            gameState = WIN;
    }
    else if (other.tag == WALL)
    {
        float hw = (SCREEN_WIDTH) * 0.5f - BRICK_WIDTH;
        float hh = BRICK_HEIGHT;
        if (CircleRect(ball.pos, BALL_RADIUS, { 0.0f, SCREEN_HEIGHT }, { hw, hh }))
        {
            ball.disabled = true;
        }
        if (ball1->disabled && ball2->disabled)
            OnReset();
    }
}
