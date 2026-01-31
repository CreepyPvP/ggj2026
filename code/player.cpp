#include "player.h"

#include "entity.h"
#include "game.h"
#include "game_math.h"
#include "raymath.h"


Player *PLAYER;

Player::Player() {
    PLAYER = this;
}

Player::~Player() {
    if (PLAYER == this) PLAYER = NULL;
}

void Player::Update(f32 delta) {
    Entity::Update(delta);

    // Update interaction system
    Entity *new_interaction_target = NULL;
    f32 best_dist = 100;
    for (u32 i = 0; i < arrlen(state.entities); ++i) {
        Entity *entity = state.entities[i];
        if (!entity->interactable) continue;
        f32 dist = Vector2LengthSqr(entity->position - this->position);
        if (dist > 1) continue;
        if (best_dist < dist) continue;
        best_dist = dist;
        new_interaction_target = entity;
    }

    if (new_interaction_target != last_interactable) {
        if (last_interactable) last_interactable->SetActiveInteraction(false);
        last_interactable = new_interaction_target;
        if (last_interactable) last_interactable->SetActiveInteraction(true);
    }


    if (IsKeyDown(KEY_SPACE) && last_interactable) {
        last_interactable->Interact(IsKeyPressed(KEY_SPACE));
    }

    // Update movement
    Vector2 movement = {};
    if (IsKeyDown(KEY_W))
        movement.y += -1;
    if (IsKeyDown(KEY_S))
        movement.y += 1;
    if (IsKeyDown(KEY_A))
        movement.x += -1;
    if (IsKeyDown(KEY_D))
        movement.x += 1;
    movement = Vector2Normalize(movement);

    // x movement
    {
        Vector2 dir = {movement.x, 0};
        f32 len = Min(GameRaycast(position, dir) - 0.001, 6 * delta);
        position += dir * len;
    }

    // y movement
    {
        Vector2 dir = {0, movement.y};
        f32 len = Min(GameRaycast(position, dir) - 0.001, 6 * delta);
        position += dir * len;
    }

    if (Vector2LengthSqr(movement) > 0) last_dir = movement;
}

void Player::Draw() {
    Entity::Draw();

    Vector2 render_pos = {floorf(this->position.x * 32), floorf(this->position.y * 32)};
    if (last_dir.y < 0) {
        DrawTextureRec(tileset, Rectangle{96, 352, 32, 32}, render_pos, WHITE);
    } else if (last_dir.y > 0) {
        DrawTextureRec(tileset, Rectangle{32, 352, 32, 32}, render_pos, WHITE);
    } else if (last_dir.x < 0) {
        DrawTextureRec(tileset, Rectangle{64, 352, 32, 32}, render_pos, WHITE);
    } else if (last_dir.x > 0) {
        DrawTextureRec(tileset, Rectangle{128, 352, 32, 32}, render_pos, WHITE);
    } else {
        DrawTextureRec(tileset, Rectangle{32, 352, 32, 32}, render_pos, WHITE);
    }
}
