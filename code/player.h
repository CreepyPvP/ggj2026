#pragma once
#include "entity.h"
#include "guard.h"

struct Player;

extern Player *PLAYER;

struct Player : Entity {

    Entity *last_interactable = NULL;
    Vector2 last_dir = {};

    Vector2 spawnPosition;

    GuardColor playerColor = Blue;

    u32 animation_frame = 0;
    f32 animation_time = 0;

    Player();
    ~Player() override;
    void Update(f32 delta) override;

    void Draw() override;
};
