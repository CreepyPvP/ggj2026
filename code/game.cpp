#include "game.h"

#include <stdlib.h>
#include <string.h>

#include "entity.h"
#include "scene.h"
#include "game_math.h"
#include "player.h"

#include "raylib.h"
#include "raymath.h"

#include "LDtkLoader/Project.hpp"

Scene *game_scene;


struct GameState
{
    u32 width;
    u32 height;
    u8 tiles[64 * 64];

    Entity **entities;
    Camera2D camera;
};

static GameState state;
    
void SetTile(i32 x, i32 y, u8 tile)
{
    if (x < 0 || y < 0 || x >= state.width || y >= state.height)
        return;
    state.tiles[x + y * state.width] = tile;
}

u8 GetTile(i32 x, i32 y)
{
    if (x < 0 || y < 0 || x >= state.width || y >= state.height)
        return 0;
    return state.tiles[x + y * state.width];
}

void LoadWorld()
{
    ldtk::Project ldtk_project;
    ldtk_project.loadFromFile("assets/world/game_world.ldtk");
    const auto& world = ldtk_project.getWorld("world");
    const auto& level = world.getLevel("level_0");
    const ldtk::Layer& collision_layer = level.getLayer("collisions");

    state.width = level.size.x / 32;
    state.height = level.size.y / 32;

    for (i32 x = 0; x < state.width; ++x)
    {
        for (i32 y = 0; y < state.height; ++y)
        {
            const ldtk::IntGridValue& grid_val = collision_layer.getIntGridVal(x, y);
            SetTile(x, y, grid_val.value > 0);
        }
    }

    for (const ldtk::Layer& layer : level.allLayers())
    {
        if (layer.getType() == ldtk::LayerType::AutoLayer)
        {
            for (const auto& tile : layer.allTiles()) {
                const auto& position = tile.getPosition();
                const auto& texture_rect = tile.getTextureRect();

                Vector2 dest = Vector2(position.x, position.y);
                Rectangle src = {
                    (f32) texture_rect.x,
                    (f32) texture_rect.y,
                    (f32) texture_rect.width * (tile.flipX ? -1.0f : 1.0f),
                    (f32) texture_rect.height * (tile.flipY ? -1.0f : 1.0f)
                };
            }
        }
    }


    // // iterate on Enemy entities
    // for (const ldtk::Entity& enemy : level1.getLayer("Entities").getEntitiesByName("Enemy")) {
    //
    //     // iterate over an array field of Enum values
    //     for (const auto& item : enemy.getArrayField<ldtk::EnumValue>("items")) {
    //         // test if field is null
    //         if (!item.is_null()) {
    //             // do something with item
    //             if (item == world.getEnum("Items")["Sword"]) {
    //                 // the enemy has a Sword !
    //             }
    //         }
    //     }
    //
    //     // get an Entity field
    //     int enemy_hp = enemy.getField<int>("HP").value();
    // }
}

static void AddEntity(Entity *entity) {
    arrput(state.entities, entity);
    entity->entityId = arrlen(state.entities) - 1;
}

static void GameSetup()
{
    memset(&state, 0, sizeof(GameState));

    state.camera.target = Vector2{ 0, 0 };
    state.camera.offset = Vector2{ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    state.camera.rotation = 0.0f;
    state.camera.zoom = 2.5f;

    // player
    Player *player = new Player();
    player->position = {2, 2};
    AddEntity(player);

    LoadWorld();
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

void UpdateCamera(const Entity *entity, f32 delta) {
        Vector2 playerPos = entity->position;
        Vector2 cameraPos = state.camera.target;

        const int targetX = 0.1f * playerPos.x * 32 + 0.9f * cameraPos.x;
        const int targetY = 0.1f * playerPos.y * 32 + 0.9f * cameraPos.y;

        state.camera.target = Vector2(targetX, targetY);
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
        Entity *entity = state.entities[i];
        entity->Update(delta);
    }

    if (PLAYER != NULL) UpdateCamera(PLAYER, delta);


    // Render
    //

    BeginDrawing();
    ClearBackground(LIGHTGRAY);
    BeginMode2D(state.camera);

    for (u32 x = 0; x < state.width; ++x)
    {
        for (u32 y = 0; y < state.height; ++y)
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
