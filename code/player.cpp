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

    // //Update Color Debug
    // if(IsKeyDown(KEY_R))
    //     playerColor = Red;
    // if(IsKeyDown(KEY_G))
    //     playerColor = Green;
    // if(IsKeyDown(KEY_B))
    //     playerColor = Blue;

    u32 color_count = 0;
    i32 color_current = 0;
    GuardColor colors[3];
    // if (state.equipment.mask_red)
    {
        if (playerColor == Red)
            color_current = color_count;
        colors[color_count++] = Red;
    }
    // if (state.equipment.mask_green)
    {
        if (playerColor == Green)
            color_current = color_count;
        colors[color_count++] = Green;
    }
    // if (state.equipment.mask_blue)
    {
        if (playerColor == Blue)
            color_current = color_count;
        colors[color_count++] = Blue;
    }

    if (IsKeyPressed(KEY_Q))
    {
        color_current--;
    }
    if (IsKeyPressed(KEY_E))
    {
        color_current++;
    }

    if (color_current < 0)
    {
        color_current = color_count - 1;
    }
    if (color_current >= color_count)
    {
        color_current = 0;
    }

    playerColor = colors[color_current];

    // j,k,l
    if (IsKeyPressed(KEY_J) || IsKeyPressed(KEY_LEFT))
    {
        playerColor = Red;
    }
    if (IsKeyPressed(KEY_K) || IsKeyPressed(KEY_UP))
    {
        playerColor = Green;
    }
    if (IsKeyPressed(KEY_L) || IsKeyPressed(KEY_RIGHT))
    {
        playerColor = Blue;
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

    // collision (player hitbox is at 0.5, 0.775)
    Vector2 sensor_offsets[] = {
        { 0.4, 0.9 },
        { 0.4, 0.65 },
        { 0.6, 0.9 },
        { 0.6, 0.65 },

        { 0.4, 0.65 },
        { 0.6, 0.65 },
        { 0.4, 0.9 },
        { 0.6, 0.9 },
    };
    f32 speed = 8;

    // x movement
    {
        Vector2 dir = {movement.x, 0};

        u32 off = 0;
        if (dir.x > 0)
            off += 2;

        f32 sensor_value = Min(GameRaycast(position + sensor_offsets[off], dir, 5) - 0.001,
                                GameRaycast(position + sensor_offsets[off + 1], dir, 5) - 0.001);
        f32 len = Min(sensor_value, speed * delta);
        position += dir * len;
    }

    // y movement
    {
        Vector2 dir = {0, movement.y};

        u32 off = 4;
        if (dir.y > 0)
            off += 2;

        f32 sensor_value = Min(GameRaycast(position + sensor_offsets[off], dir, 5) - 0.001,
                                GameRaycast(position + sensor_offsets[off + 1], dir, 5) - 0.001);
        f32 len = Min(sensor_value, speed * delta);
        position += dir * len;
    }

    if (Vector2LengthSqr(movement) > 0) last_dir = movement;

    // animation
    if (animation_time > 0.1)
    {
        animation_frame = animation_frame? 0 : 1;
        animation_time -= 0.1;
    }
    if (Vector2LengthSqr(movement) > 0.1)
        animation_time += delta;
}

void Player::Draw() {
    Entity::Draw();

    float texture_y = 576;
    switch(playerColor){
        case Red: texture_y = 608; break;
        case Blue: texture_y = 672; break;
        case Green: texture_y = 736; break;
        default: texture_y = 576; break;
    }

    Vector2 render_pos = {floorf(this->position.x * 32), floorf(this->position.y * 32)};
    DrawTextureRec(tileset, Rectangle{32, 416, 32, 32}, render_pos + Vector2{0.0, 15}, WHITE);

    if (animation_frame)
    {
        texture_y += 32;
    }

    if (last_dir.y < 0) {
        DrawTextureRec(tileset, Rectangle{96, texture_y, 32, 32}, render_pos, WHITE);
    } else if (last_dir.y > 0) {
        DrawTextureRec(tileset, Rectangle{32, texture_y, 32, 32}, render_pos, WHITE);
    } else if (last_dir.x < 0) {
        DrawTextureRec(tileset, Rectangle{64, texture_y, 32, 32}, render_pos, WHITE);
    } else if (last_dir.x > 0) {
        DrawTextureRec(tileset, Rectangle{128, texture_y, 32, 32}, render_pos, WHITE);
    } else {
        DrawTextureRec(tileset, Rectangle{32, texture_y, 32, 32}, render_pos, WHITE);
    }
}
