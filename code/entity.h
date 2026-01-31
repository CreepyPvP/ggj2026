#pragma once
#include "raylib.h"
#include "types.h"

#define INVALID_ENTITY_ID (-1)

struct Entity {
    virtual ~Entity() = default;

    u32 entity_id = INVALID_ENTITY_ID;
    Vector2 position = {};

    bool interactable;
    bool active_interaction;;

    virtual void Update(f32 delta) {}
    virtual void Draw() {}
    virtual void Interact() {}
    virtual void SetActiveInteraction(bool active) {active_interaction=active;}
};
