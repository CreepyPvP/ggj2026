#pragma once
#include "entity.h"

struct Player;

extern Player *PLAYER;

struct Player : Entity {

    Entity *last_interactable = NULL;
    Vector2 last_dir = {};

    Player();
    ~Player() override;
    void Update(f32 delta) override;

    void Draw() override;
};
