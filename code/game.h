#pragma once
#include "raylib.h"
#include "types.h"

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

struct GameState
{
    Room rooms[32];
    u32 room_count;

    TexturedTile *textured_tiles;

    Entity **entities;
    Camera2D camera;
};

extern GameState state;

f32 GameRaycast(Vector2 pos, Vector2 dir);

extern Scene *game_scene;
void GameInitialize();
void AddEntity(Entity *entity);
