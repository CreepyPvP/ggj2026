#pragma once
#include "raylib.h"
#include "types.h"

struct Entity;
struct Scene;

struct GameState
{
    u32 width;
    u32 height;
    u8 tiles[64 * 64];

    Entity **entities;
    Camera2D camera;
};

extern GameState state;


extern Scene *game_scene;
void GameInitialize();
