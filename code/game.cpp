#include "game.h"

#include <stdlib.h>
#include <string.h>

#include "scene.h"
#include "game_math.h"

#include "raylib.h"
#include "raymath.h"

Scene *game_scene;

struct Entity
{
    Vector2 position;
    bool possesed;
};

struct GameState
{
    u32 width;
    u32 height;
    u8 tiles[32 * 32];

    Entity *entities;
};

static GameState state;

void SetTile(u32 x, u32 y, u8 tile)
{
    state.tiles[x + y * state.width] = tile;
}

u8 GetTile(u32 x, u32 y)
{
    return state.tiles[x + y * state.width];
}

static void GameSetup()
{
    memset(&state, 0, sizeof(GameState));

    state.width = 10;
    state.height = 10;

    for (u32 x = 0; x < 10; ++x)
    {
        for (u32 y = 0; y < 10; ++y)
        {
            if (!x || x == 9 || !y || y == 9 || (x == 3 && y == 3))
            {
                SetTile(x, y, 1);
            }
        }
    }

    // player
    Entity player = {};
    player.possesed = true;
    player.position = {2, 2};
    arrput(state.entities, player);
}

static void GameDestroy()
{
    arrfree(state.entities);
}

static f32 Raycast(Vector2 pos, Vector2 dir)
{
    u32 tile_x = pos.x;
    u32 tile_y = pos.y;
    if (GetTile(tile_x, tile_y))
    {
        return 0;
    }
    pos += dir;
    return 0;
}

static void DoEntityMovement(Entity *entity, f32 delta)
{
    Vector2 movement = {};
    if (IsKeyDown(KEY_W))
        movement.y += -1;
    if (IsKeyDown(KEY_S))
        movement.y += 1;
    if (IsKeyDown(KEY_A))
        movement.x += -1;
    if (IsKeyDown(KEY_D))
        movement.x += 1;
    movement = Vector2Normalize(movement);
    entity->position += movement * 10 * delta;
}

static void GameFrame(f32 delta)
{
    // Update
    // 

    for (u32 i = 0; i < arrlen(state.entities); ++i)
    {
        Entity *entity = state.entities;
        if (entity->possesed)
        {
            DoEntityMovement(entity, delta);
        }
    }

    // Render
    //

    BeginDrawing();
    ClearBackground(LIGHTGRAY);

    for (u32 x = 0; x < 10; ++x)
    {
        for (u32 y = 0; y < 10; ++y)
        {
            u8 tile = GetTile(x, y);

            if (tile)
                DrawRectangle(x * 32, y * 32, 32, 32, BLACK);  
        }
    }

    for (u32 i = 0; i < arrlen(state.entities); ++i)
    {
        Entity *entity = state.entities + i;
        DrawRectangle(entity->position.x * 32, entity->position.y * 32, 32, 32, BLUE);  
    }

    {
        // fade in
        f32 t = Range(game_scene->time, 0, 0.5);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 1 - t));                        
    }

    EndDrawing();
}

void GameInitialize()
{
    game_scene = (Scene *) malloc(sizeof(Scene));
    memset(game_scene, 0, sizeof(*game_scene));
    game_scene->Frame = GameFrame;
    game_scene->Setup = GameSetup;
    game_scene->Destroy = GameDestroy;
}
