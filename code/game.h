#pragma once
#include "raylib.h"
#include "types.h"
#include "LDtkLoader/Entity.hpp"

struct Entity;
struct Scene;

extern Texture2D tileset;

struct TexturedTile
{
    Vector2 position;
    Rectangle source;
};

struct Room {
    i32 offset_x;
    i32 offset_y;
    u32 width;
    u32 height;
    u8 tiles[128 * 128];
};

struct Equipment {
    bool mask_red;
    bool mask_green;
    bool mask_blue;

    u32 lock_picks;
    bool glass_cutter;
};

struct GameState
{
    Room *rooms;

    u32 saved_cash;
    u32 held_cash;

    TexturedTile *textured_tiles;

    Entity **entities;
    Camera2D camera;

    Equipment equipment;

    //
    // loose sequence

    bool game_lost;
    f32 time_since_game_lost;
};

extern GameState state;

f32 GameRaycast(Vector2 pos, Vector2 dir, f32 max_dist);
void GameDrawCone(Vector2 pos, f32 forward_angle, f32 length, f32 angle, Color color);
void GameStartLose();

extern Scene *game_scene;
void GameInitialize();
void AddEntity(Entity *entity);
void SetTile(float x, float y, int value);
void SetTile(i32 int_x, i32 int_y, Room* room, int value);
Entity* getEntity(ldtk::IID ldtk_id);
