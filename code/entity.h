#pragma once
#include "raylib.h"
#include "types.h"

#define INVALID_ENTITY_ID 999999999

struct Entity {
    virtual ~Entity() = default;

    u32 entityId = INVALID_ENTITY_ID;
    Vector2 position = {};

    virtual void Update(f32 delta) {}
    virtual void Draw() {}
};
