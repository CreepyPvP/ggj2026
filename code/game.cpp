#include "game.h"

#include <stdlib.h>
#include <string.h>

#include "entity.h"
#include "scene.h"
#include "game_math.h"
#include "player.h"

#include "raylib.h"
#include "LDtkLoader/Project.hpp"
#include "raymath.h"

Scene *game_scene;


struct GameState
{
    u32 width;
    u32 height;
    u8 tiles[32 * 32];

    Entity **entities;
    Camera2D camera;
};

static GameState state;
    
    void doSum(){
        ldtk::Project ldtk_project;
    try {
        ldtk_project.loadFromFile("assets/test/testWorld.ldtk");
    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }


    // const auto& world = ldtk_project.getWorld();
    //
    // auto worlds = ldtk_project.allWorlds();
    //
    // // Drawing level 1
    // auto levels = worlds[0].allLevels();
    // auto layers = levels[0].allLayers();
    //
    // for (auto& layer : layers) {
    //     Texture2D texture = LoadTexture(("assets/" + layer.getTileset().path).c_str());
    //     RenderTexture2D renderer = LoadRenderTexture(levels[0].size.x, levels[0].size.y);
    //     auto& tiles = layer.allTiles();
    //
    //     BeginTextureMode(renderer);
    //     ClearBackground(BLACK);
    //
    //     for (const auto& tile : tiles) {
    //         const auto& position = tile.getPosition();
    //         const auto& texture_rect = tile.getTextureRect();
    //
    //         Vector2 dest = Vector2(static_cast<float>(position.x), static_cast<float>(position.y));
    //         Rectangle src = {
    //             static_cast<float>(texture_rect.x),
    //             static_cast<float>(texture_rect.y),
    //             static_cast<float>(texture_rect.width) * (tile.flipX ? -1.0f : 1.0f),
    //             static_cast<float>(texture_rect.height) * (tile.flipY ? -1.0f : 1.0f)
    //         };
    //
    //         DrawTextureRec(texture, src, dest, WHITE);
    //     }
    //     EndTextureMode();
    // }
    }


static void AddEntity(Entity *entity) {
    arrput(state.entities, entity);
    entity->entityId = arrlen(state.entities) - 1;
}


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
        
    state.camera.target = Vector2{ 0, 0 };
    state.camera.offset = Vector2{ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    state.camera.rotation = 0.0f;
    state.camera.zoom = 2.5f;

    // player
    Player *player = new Player();
    player->position = {2, 2};
    AddEntity(player);
}

static void GameDestroy()
{
    for (u32 i = 0; i < arrlen(state.entities); ++i)
    {
        Entity *entity = state.entities[i];
        delete entity;
    }
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

static void GameFrame(f32 delta)
{
    // Update
    // 

    for (u32 i = 0; i < arrlen(state.entities); ++i)
    {
        Entity *entity = state.entities[i];
        entity->Update(delta);
    }

    // Render
    //

    BeginDrawing();
    ClearBackground(LIGHTGRAY);
    BeginMode2D(state.camera);

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
        Entity *entity = state.entities[i];
        entity->Draw();
    }

    // for (u32 i = 0; i < arrlen(state.entities); ++i)
    // {
    //     Entity *entity = state.entities + i;
    //     DrawRectangle(entity->position.x * 32, entity->position.y * 32, 32, 32, BLUE);
    // }


    EndMode2D();

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
    game_scene->Setup = GameSetup;
    game_scene->Frame = GameFrame;
    game_scene->Destroy = GameDestroy;
}
