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
Texture2D tileset;

GameState state;
    
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
    const auto& world = ldtk_project.getWorld("tutorial");
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

                TexturedTile textured = {};
                textured.position = Vector2(position.x, position.y);
                textured.source = {
                    (f32) texture_rect.x,
                    (f32) texture_rect.y,
                    (f32) texture_rect.width * (tile.flipX ? -1.0f : 1.0f),
                    (f32) texture_rect.height * (tile.flipY ? -1.0f : 1.0f)
                };

                arrput(state.textured_tiles, textured);
            }
        }
    }

    for (const ldtk::Layer& layer : level.allLayers()) {
        if (layer.getType() != ldtk::LayerType::Entities) continue;

        for (auto& data_entity : layer.allEntities()) {
            Entity *entity = NULL;
            if (data_entity.getName() == "door") {
                // entity = new Door()
            }


            if (entity) {
                entity->position = Vector2{(f32)data_entity.getWorldPosition().x, (f32)data_entity.getWorldPosition().y};
                entity->Configure(data_entity);
                AddEntity(entity);
            }
        }
    }

    // iterate on Enemy entities
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

void AddEntity(Entity *entity) {
    arrput(state.entities, entity);
    entity->entity_id = arrlen(state.entities) - 1;
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
    arrfree(state.textured_tiles);
}

f32 GameRaycast(Vector2 pos, Vector2 dir) 
{
    f32 min_t = 1e30;

    for (u32 y = 0; y < state.height; ++y) 
    {
        for (u32 x = 0; x < state.width; ++x)
        {
            if (GetTile(x, y) == 0)
                continue;

            Vector2 aabb_position = { (f32) x, (f32) y };
            Vector2 aabb_size = { 1, 1 };

            // x collision
            if (dir.x > 0.00001 || dir.x < 0.00001) 
            {
                {
                    float t = (aabb_position.x - pos.x) / dir.x;
                    float y = t * dir.y + pos.y;
                    float y_diff = y - aabb_position.y;

                    if (y_diff >= 0 && y_diff <= aabb_size.y) {
                        if (t >= 0 && t < min_t) {
                            min_t = t;
                        }
                    }
                } 
                {
                    float t = (aabb_position.x + aabb_size.x - pos.x) / dir.x;
                    float y = t * dir.y + pos.y;
                    float y_diff = y - aabb_position.y;

                    if (y_diff >= 0 && y_diff <= aabb_size.y) {
                        if (t >= 0 && t < min_t) {
                            min_t = t;
                        }
                    }
                }
            }

            // y collision
            if (dir.y > 0.00001 || dir.y < 0.00001) {
                {
                    float t = (aabb_position.y - pos.y) / dir.y;
                    float x = t * dir.x + pos.x;
                    float x_diff = x - aabb_position.x;

                    if (x_diff >= 0 && x_diff <= aabb_size.x) {
                        if (t >= 0 && t < min_t) {
                            min_t = t;
                        }
                    }
                } 
                {
                    float t = (aabb_position.y + aabb_size.y - pos.y) / dir.y;
                    float x = t * dir.x + pos.x;
                    float x_diff = x - aabb_position.x;

                    if (x_diff >= 0 && x_diff <= aabb_size.x) {
                        if (t >= 0 && t < min_t) {
                            min_t = t;
                        }
                    }
                }
            }
        }
    }

    return min_t;
}

void UpdateCamera(const Entity *entity, f32 delta) 
{
    Vector2 playerPos = entity->position;
    Vector2 cameraPos = state.camera.target;

    // Smoothing camera following player
    float targetX = 0.1f * playerPos.x * 32 + 0.9f * cameraPos.x;
    float targetY = 0.1f * playerPos.y * 32 + 0.9f * cameraPos.y;

    // Centering camera
    targetX++;
    targetY++;

    state.camera.target = Vector2(targetX, targetY);
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

    for (u32 i = 0; i < arrlen(state.textured_tiles); ++i)
    {
        TexturedTile textured = state.textured_tiles[i];
        DrawTextureRec(tileset, textured.source, textured.position, WHITE);
    }

    // Draw colliders
    // for (u32 x = 0; x < state.width; ++x)
    // {
    //     for (u32 y = 0; y < state.height; ++y)
    //     {
    //         u8 tile = GetTile(x, y);
    //
    //         if (tile)
    //             DrawRectangle(x * 32, y * 32, 32, 32, BLACK);  
    //     }
    // }

    for (u32 i = 0; i < arrlen(state.entities); ++i)
    {
        Entity *entity = state.entities[i];
        entity->Draw();
    }


    EndMode2D();

    {
        // fade in
        f32 t = Range(game_scene->time, 0, 0.65);
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
