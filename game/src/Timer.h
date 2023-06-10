#pragma once

struct Timer
{
    float elapsed = 0.0f;
    float duration = 0.0f;

    float Percent() { return elapsed / duration; }
    bool Expired() { return elapsed >= duration; }
    void Reset() { elapsed = 0.0f; }
    void Tick(float dt) { elapsed += dt; }
};
