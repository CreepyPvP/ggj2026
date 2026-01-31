#pragma once

#include "raylib.h"
#include "types.h"
#include "game.h"
#include "LDtkLoader/Entity.hpp"

#define INVALID_ENTITY_ID (-1)

struct Entity {
    virtual ~Entity() = default;


    u32 entity_id = INVALID_ENTITY_ID;
    Vector2 position = {};

    bool interactable = false;
    bool active_interaction = NULL;

    virtual void Configure(const ldtk::World &world, Room* room, const ldtk::Entity &data) {
    }

    virtual void Update(f32 delta) {
    }

    virtual void Draw() {
    }

    ///
    /// @param pressed true if pressed this frame, false if held down
    virtual void Interact(bool pressed) {
    }

    virtual void SetActiveInteraction(bool active) { active_interaction = active; }
};
