#include "extract.h"
#include "game.h"
#include "player.h"
#include "raymath.h"
#include "LDtkLoader/World.hpp"

void Extract::Update(f32 delta) {
    Entity::Update(delta);

    if (Vector2DistanceSqr(position, PLAYER->position) < 0.8 * 0.8) {
        state.saved_cash += state.held_cash;
        state.held_cash = 0;
    }
}

void Extract::Draw() {
    Entity::Draw();

    Vector2 render_pos = {floorf(this->position.x * 32), floorf(this->position.y * 32)};
    DrawTextureRec(tileset, Rectangle{192, 896, 64, 32}, render_pos, WHITE);
}


void Extract::Configure(const ldtk::World &world, Room* room, const ldtk::Entity &data) {
    Entity::Configure(world, room, data);
}
