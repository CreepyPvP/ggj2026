#include "treasure.h"

#include "game.h"
#include "player.h"
#include "raymath.h"

void Treasure::Update(f32 delta) {
    Entity::Update(delta);

    if (!PLAYER) return;
    if (collected) return;

    if (Vector2DistanceSqr(position, PLAYER->position) < 0.8 * 0.8) {
        collected = true;
        state.held_cash += 10;
    }
}

void Treasure::Draw()  {
    Entity::Draw();
    if (collected) return;

    Vector2 render_pos = {floorf(this->position.x * 32), floorf(this->position.y * 32)};
    DrawTextureRec(tileset, Rectangle(96, 384,32,32),render_pos, WHITE);
}


