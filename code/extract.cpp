#include "extract.h"
#include "game.h"
#include "player.h"
#include "raymath.h"
#include "LDtkLoader/World.hpp"

void Extract::Update(f32 delta) {
    Entity::Update(delta);

    if (Vector2DistanceSqr(position, PLAYER->position) < 0.8 * 0.8) {
        state.game_lost = true;
    }
}

void Extract::Draw() {
    Entity::Draw();

    Vector2 render_pos = {floorf(this->position.x * 32), floorf(this->position.y * 32)};
    DrawCircle(render_pos.x, render_pos.y, 32, RED);
}


void Extract::Configure(const ldtk::World &world, Room* room, const ldtk::Entity &data) {
    Entity::Configure(world, room, data);
}
