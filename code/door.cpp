#include "door.h"

#include "math.h"
#include "game.h"
#include "game_math.h"
#include "raymath.h"
#include "rlgl.h"
#include "LDtkLoader/World.hpp"


void Door::Update(f32 delta) {
    if (animation_timer >= 0) {
        animation_timer += 2 * delta;
        if (animation_timer >= 2)
            animation_timer = -1;
    }

    if (unlocked || !unlockable || !active_interaction) return;

    state.target_zoom = 3.5 + unlock_step * 1;
    state.target_vignette = 1 + unlock_step * 0.1;

    if (unlock_error_timer > 0) {
        unlock_error_timer -= delta;
        return;
    }
    unlock_cursor += delta * unlock_cursor_speed;

    if (unlock_cursor < 0) {
        unlock_cursor = -unlock_cursor;
        unlock_cursor_speed = -unlock_cursor_speed;
    }
    if (unlock_cursor > 1) {
        unlock_cursor =  2 - unlock_cursor;
        unlock_cursor_speed = -unlock_cursor_speed;
    }

    if (IsKeyPressed(KEY_SPACE)) {
        if (unlock_cursor >= unlock_offset && unlock_cursor <= unlock_offset + unlock_range) {
            unlock_step++;
            SelectRandomUnlockRange();

            if (unlock_step > 3) {
                PlaySound(switchSound);
                Open();
                state.target_zoom = 2.5;
            }else{
                PlaySound(slideSound);
            }
        } else {
            PlaySound(failureSound);
            unlock_error_timer = 1;
        }
    }

    if (IsKeyPressed(KEY_F) && state.equipment.lock_picks > 0) {
        state.equipment.lock_picks--;
        Open();
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

void Door::PostDraw() {
    Entity::PostDraw();
    if (unlocked) return;
    if (!active_interaction) return;

    Color shapeColor = {31, 33, 44, 255};
    if(unlock_error_timer > 0 && ((int)(unlock_error_timer * 10)) % 2 == 0  ){
        shapeColor = {240,33,44,255};
    }

    Vector2 render_pos = {floorf(this->position.x * 32) + 16, floorf(this->position.y * 32)};
    render_pos = Vector2{render_pos.x, render_pos.y - 8};
    Vector2 shape = {32, 8};
    Vector2 inner_shape = {28, 4};
    //Outer shape drawing
    DrawRectangleRec(Rectangle{render_pos.x - shape.x / 2, render_pos.y - shape.y / 2, shape.x, shape.y}, shapeColor);
    //Innter shape drawing
    DrawRectangleRec(Rectangle{render_pos.x - inner_shape.x / 2, render_pos.y - inner_shape.y / 2, inner_shape.x, inner_shape.y}, {83, 82, 106, 255});

    {
        //Unlock range
        Vector2 cursor_pos = {render_pos.x - inner_shape.x / 2, render_pos.y - inner_shape.y / 2};

        cursor_pos = Vector2Add(cursor_pos, Vector2{(inner_shape.x - 1) * unlock_offset, 0});

        DrawRectangleRec(Rectangle{cursor_pos.x, cursor_pos.y, ceilf(inner_shape.x * unlock_range), inner_shape.y}, {216, 149, 87, 255});
    }

    {
        //Cursor
        Vector2 cursor_pos = {render_pos.x - inner_shape.x / 2, render_pos.y - inner_shape.y / 2};

        cursor_pos = Vector2Add(cursor_pos, Vector2{(inner_shape.x - 1) * unlock_cursor, 0});
        Color cursor_color = {99, 149, 121, 255};
        if(unlock_error_timer>0)
            cursor_color = {255,0,0 ,255}; 
        DrawRectangleRec(Rectangle{cursor_pos.x, cursor_pos.y, 1, inner_shape.y}, cursor_color);
    }

    {
        const char *text = "- \"space\"";

        Vector2 pos = {render_pos.x + shape.x/2 + 3, render_pos.y - shape.y/2};
        Vector2 shape = MeasureTextEx(GetFontDefault(), text, 6, 1);
        DrawRectangle(pos.x - 2, pos.y - 1, shape.x + 4, shape.y + 2, ColorAlpha(BLACK, 0.5f));
        DrawTextEx(GetFontDefault(), text, pos, 6, 1, WHITE);
    }

    if (state.equipment.lock_picks > 0) {
        const char *text = "Bypass with Lockpick 'F' x%i";

        text = TextFormat(text, state.equipment.lock_picks);

        Vector2 pos = {render_pos.x, render_pos.y + 7};
        Vector2 shape = MeasureTextEx(GetFontDefault(), text, 6, 1);

        pos.x -= shape.x/2;

        DrawRectangle(pos.x - 1, pos.y - 1, shape.x + 3, shape.y + 2, ColorAlpha(BLACK, 0.5f));
        DrawTextEx(GetFontDefault(), text, pos, 6, 1, ORANGE);
    }


}


void Door::Open() {
    interactable = false;
    unlocked = true;
    SetTile(position.x, position.y, 0);

    animation_timer = 0;
}


void Door::Configure(const ldtk::World &world, Room *room, const ldtk::Entity &data) {
    unlockable = data.getField<ldtk::FieldType::Bool>("unlockable").value();

    interactable = unlockable;
}

void Door::SetActiveInteraction(bool active) {
    Entity::SetActiveInteraction(active);

    // reset unlock system
    unlock_step = 0;
    unlock_offset = 0.8f;
    unlock_range = 0.2f;
    unlock_cursor = 0;
    unlock_cursor_speed = 1;
    unlock_error_timer = 0;

    if (!active)
    {
        state.target_zoom = 2.5;
        state.target_vignette = 0.12;
    }

    SelectRandomUnlockRange();
}

void Door::SelectRandomUnlockRange() {


    unlock_cursor_speed = 1 + unlock_step * 0.3f;
    u32 difficulty = 30 - unlock_step * 5;
    unlock_range = difficulty / 100.0f;
    unlock_offset = std::rand() % (100 - difficulty) / 100.0f;
}
