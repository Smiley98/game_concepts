#include "rlImGui.h"
#include "Math.h"
#include <iostream>
#include <vector>
#include <algorithm>
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

bool InFoV(Vector2 viewerPosition, Vector2 viewerDirection, float viewDistance, float FoV/*radians*/, Vector2 targetPosition)
{
    // Don't bother with angle-check if target is far away from viewer
    if (Distance(viewerPosition, targetPosition) > viewDistance) return false;
    // Optimization: compare squared distances to avoid expensive square-root calculations
    //if (DistanceSqr(viewerPosition, targetPosition) > viewDistance * viewDistance) return false;

    // Vector from A to B = B - A
    Vector2 targetDirection = Normalize(targetPosition - viewerPosition);

    // Determine if the angle between viewer and target (dot product) is less than
    // half the viewer's field of view (half to form a right-triangle)
    float angle = acosf(Dot(viewerDirection, targetDirection));
    return angle <= FoV * 0.5f;

    // Optimization: Since arccos is the inverse of cos, we can flip the comparison to avoid an inverse trig function
    //return Dot(viewerDirection, targetDirection) > cosf(FoV * 0.5f);
}

struct Entity
{
    Vector2 dir{ 1.0f, 0.0f };  // right
    Vector2 pos{};
    Vector2 vel{};

    float radius = 0.0f;
    float health = 100.0f;
};

struct Timer
{
    float duration = 0.0f;
    float elapsed = 0.0f;

    bool Expired() { return elapsed >= duration; }
    void Reset() { elapsed = 0.0f; }
    void Tick(float dt) { elapsed += dt; }
};

struct Node
{
    Entity* self = nullptr;
    Entity* target = nullptr;
    virtual Node* Evaluate() = 0;
};

struct Action : public Node
{
    Node* Evaluate() override { return nullptr; }
};

struct Decision : public Node
{
    Node* yes = nullptr;
    Node* no = nullptr;
};

struct VisibleDecision : public Decision
{
    float viewDistance = 0.0f;
    float FoV = 0.0f;

    Node* Evaluate() final
    {
        return InFoV(self->pos, self->dir, viewDistance, FoV, target->pos) ? yes : no;
    }
};

struct RotateAction : public Action
{
    float angularSpeed = 0.0f;

    Node* Evaluate() final
    {
        self->dir = Rotate(self->dir, angularSpeed * GetFrameTime());
        return Action::Evaluate();
    }
};

struct ShootAction : public Action
{
    std::vector<Entity>* bullets = nullptr;
    Timer cooldown;

    Node* Evaluate() final
    {
        cooldown.Tick(GetFrameTime());
        if (cooldown.Expired())
        {
            cooldown.Reset();

            Entity bullet;
            bullet.health = 10.0f;
            bullet.radius = 10.0f;
            bullet.dir = Rotate(Normalize(target->pos - self->pos), Random(-25.0f, 25.0f) * DEG2RAD);
            bullet.pos = self->pos + bullet.dir * (self->radius + bullet.radius);
            bullet.vel = bullet.dir * Random(500.0f, 750.0f);
            bullets->push_back(bullet);
        }
        return Action::Evaluate();
    }
};

void Traverse(Node* node)
{
    if (node != nullptr)
    {
        Traverse(node->Evaluate());
    }
}

void Init(Node* node, Entity* self, Entity* target)
{
    node->self = self;
    node->target = target;
}

// This automatically calls the above function for each node in our decision tree
using NodeFunction = void(*)(Node* node, Entity* self, Entity* target);
void Visit(Node* node, Entity* self, Entity* target, NodeFunction function)
{
    if (node != nullptr)
    {
        function(node, self, target);

        Decision* decision = dynamic_cast<Decision*>(node);
        if (decision != nullptr)
        {
            Visit(decision->yes, self, target, function);
            Visit(decision->no, self, target, function);
        }
    }
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
    SetTargetFPS(60);
    rlImGuiSetup(true);

    float FoV = 60.0f * DEG2RAD;
    float viewDistance = 500.0f;
    std::vector<Entity> bullets;

    Entity player, enemy;
    enemy.pos = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
    player.radius = enemy.radius = 25.0f;

    VisibleDecision isVisible;
    isVisible.FoV = FoV;
    isVisible.viewDistance = viewDistance;

    RotateAction rotateAction;
    rotateAction.angularSpeed = 100.0f * DEG2RAD;

    ShootAction shootAction;
    shootAction.bullets = &bullets;
    shootAction.cooldown.duration = 0.01f;

    isVisible.no = &rotateAction;
    isVisible.yes = &shootAction;

    Node* root = &isVisible;
    Visit(root, &enemy, &player, Init);

    int kills = 0;
    while (!WindowShouldClose())
    {
        player.pos = GetMousePosition();
        Traverse(root);
        const Color visibleColor = InFoV(enemy.pos, enemy.dir, viewDistance, FoV, player.pos) ? RED : GREEN;

        for (Entity& bullet : bullets)
            bullet.pos = bullet.pos + bullet.vel * GetFrameTime();

        bullets.erase(
            std::remove_if(
                bullets.begin(), bullets.end(), [&player, &enemy](const Entity& bullet)
                {
                    bool playerCollision = CheckCollisionCircles(bullet.pos, bullet.radius, player.pos, player.radius);
                    if (playerCollision)
                        player.health -= bullet.health;

                    bool offScreen = !CheckCollisionCircleRec(bullet.pos, bullet.radius, { 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT });
                    return playerCollision || offScreen;
                }
            ),
            bullets.end());

        if (player.health <= 0.0f)
        {
            ++kills;
            player.health = 100.0f;
            enemy.dir = Normalize(enemy.pos - player.pos);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Render player
        DrawCircleV(player.pos, player.radius, visibleColor);

        // Render enemy
        DrawCircleV(enemy.pos, enemy.radius, BLUE);
        DrawLineV(enemy.pos, enemy.pos + Rotate(enemy.dir, FoV * 0.5f) * viewDistance, visibleColor);
        DrawLineV(enemy.pos, enemy.pos + Rotate(enemy.dir, FoV * -0.5f) * viewDistance, visibleColor);

        // Render bullets
        for (const Entity& bullet : bullets)
            DrawCircleV(bullet.pos, bullet.radius, BLUE);

        // Render instructions
        DrawText("Move the player with your cursor. If the enemy sees you... RUN!", 10, 10, 20, BLUE);
        DrawText(TextFormat("Enemy kills: %i", kills), 10, 30, 20, RED);
        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}
