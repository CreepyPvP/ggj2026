#include "door.h"

#include "game.h"
#include "game_math.h"
#include "rlgl.h"
#include "LDtkLoader/World.hpp"


void Door::Update(f32 delta) {
    if (animation_timer >= 0) {
        animation_timer += 2 * delta;
        if (animation_timer >= 2)
            animation_timer = -1;
    }
}


void Door::PreDraw() {
    Entity::Draw();
    Rectangle uv = unlockable ? Rectangle{416, 512, 32, 32} : Rectangle{416, 576, 32, 32};

    Vector2 render_pos = {floorf(this->position.x * 32), floorf(this->position.y * 32)};
    if (unlocked) {
        if (animation_timer >= 0) {
            if (animation_timer <= 1) {
                float t = Range(animation_timer, 0, 1);
                t = EaseOutExpo(t);
                rlPushMatrix();
                rlTranslatef(render_pos.x + 16, render_pos.y + 16, 0);
                rlScalef(1 + t, 1 + t, 1);
                rlTranslatef(-render_pos.x - 16, -render_pos.y - 16, 0);
            } else {
                float t = Range(animation_timer - 1, 0, 1);
                t = EaseOutExpo(1 - t);
                rlPushMatrix();
                rlTranslatef(render_pos.x + 16, render_pos.y + 16, 0);
                rlScalef(2 * t, 2 * t, 1);
                rlTranslatef(-render_pos.x - 16, -render_pos.y - 16, 0);
            }

            DrawTextureRec(tileset, uv, render_pos, {255, 255, 255, 255});
            rlPopMatrix();
        }
        return;
    }

    DrawTextureRec(tileset, uv, render_pos, {255, 255, 255, 255});
}

void Door::Open() {
    unlocked = true;
    unlockable = false;
    SetTile(position.x, position.y, 0);

    animation_timer = 0;
}


void Door::Configure(const ldtk::World &world, Room *room, const ldtk::Entity &data) {
    unlockable = data.getField<ldtk::FieldType::Bool>("unlockable").value();

    interactable = unlockable;
}
