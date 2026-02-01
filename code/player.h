#pragma once
#include "entity.h"
#include "guard.h"

struct Player;

extern Player *PLAYER;

struct Player : Entity {

    Entity *last_interactable = NULL;
    Vector2 last_dir = {};

    GuardColor playerColor = Blue;

    Player();
    ~Player() override;
    void Update(f32 delta) override;

    void Draw() override;
};
