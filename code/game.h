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

struct GameState
{
    u32 width;
    u32 height;
    u8 tiles[64 * 64];

    TexturedTile *textured_tiles;

    Entity **entities;
    Camera2D camera;
};

extern GameState state;


extern Scene *game_scene;
void GameInitialize();
void AddEntity(Entity *entity);
