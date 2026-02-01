#include "treasure.h"

#include "game.h"
#include "player.h"
#include "raymath.h"
#include "LDtkLoader/World.hpp"

void Treasure::Update(f32 delta) {
    Entity::Update(delta);

    if (!PLAYER || state.game_lost) return;
    if (collected) {
        if(animation_timer >= 0){
            animation_timer += 2*delta;
            if(animation_timer >= 1)
                animation_timer = -1;
        }
        return;
    }

    if (Vector2DistanceSqr(position, PLAYER->position) < 0.8 * 0.8) {
        collected = true;
        animation_timer = 0;
        int cash_amount = 10;
        switch(type){
            case money_small: cash_amount = 10; break;
            case money_big: cash_amount = 30; break;
            case coins_small: cash_amount = 20; break;
            case coins_big: cash_amount = 50; break;
            case diamond_small: cash_amount = 80; break;
            case diamond_big: cash_amount = 150; break;
        }

        state.held_cash += cash_amount;
    }
}

void Treasure::Draw() {
    Entity::Draw();
    if (collected) {
        if(animation_timer >= 0){
            Vector2 render_pos = {floorf(this->position.x * 32), floorf(this->position.y * 32) - 16*animation_timer};    
            DrawTextureRec(tileset, {160,544,32,32}, render_pos, WHITE);
        }
        return;
    }

    Vector2 render_pos = {floorf(this->position.x * 32), floorf(this->position.y * 32)};
    Rectangle uv_coord = {};

    Rectangle moneySmallPos = {64, 384, 32, 32};
    Rectangle coinsSmallPos = {96, 384, 32, 32};
    Rectangle diamondSmallPos = {128, 384, 32, 32};
    Rectangle moneyBigPos = {160, 384, 32, 32};
    Rectangle coinsBigPos = {192, 384, 32, 32};
    Rectangle diamondBigPos = {224, 384, 32, 32};

    switch (type) {
        case money_small: uv_coord = moneySmallPos; break;
        case coins_small: uv_coord = coinsSmallPos; break;
        case diamond_small: uv_coord = diamondSmallPos; break;
        case money_big: uv_coord = moneyBigPos; break;
        case coins_big: uv_coord = coinsBigPos; break;
        case diamond_big: uv_coord = diamondBigPos; break;
    }
    DrawTextureRec(tileset, uv_coord, render_pos, WHITE);
}


void Treasure::Configure(const ldtk::World &world, Room* room, const ldtk::Entity &data) {
    Entity::Configure(world, room, data);

    const auto &enum_value = data.getField<ldtk::EnumValue>("treasure_type");

    if (enum_value == world.getEnum("treasure_type")["money_big"]) type = money_big;
    else if (enum_value == world.getEnum("treasure_type")["coins_big"]) type = coins_big;
    else if (enum_value == world.getEnum("treasure_type")["diamong_big"]) type = diamond_big;
    else if (enum_value == world.getEnum("treasure_type")["money_small"]) type = money_small;
    else if (enum_value == world.getEnum("treasure_type")["coins_small"]) type = coins_small;
    else if (enum_value == world.getEnum("treasure_type")["diamond_small"]) type = diamond_small;
}
