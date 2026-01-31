#include "game.h"

#include <stdlib.h>
#include <string.h>

#include "entity.h"
#include "scene.h"
#include "game_math.h"
#include "guard.h"
#include "player.h"

#include "raylib.h"
#include "raymath.h"
#include "treasure.h"

#include "LDtkLoader/Project.hpp"

Scene *game_scene;
Texture2D tileset;

GameState state;

u8 GetTile(i32 x, i32 y, Room *room)
{
    i32 rx = x - room->offset_x;
    i32 ry = y - room->offset_y;
    if (rx < 0 || ry < 0 || rx >= room->width || ry >= room->height)
        return 0;

    return room->tiles[rx + ry * room->width];
}

u8 GetTile(i32 x, i32 y)
{
    for (u32 i = 0; i < arrlen(state.rooms); ++i)
    {
        Room *room = state.rooms + i;
        if (x >= room->offset_x && y >= room->offset_y &&
            x < room->offset_x + room->width && y < room->offset_y + room->height)
            return GetTile(x, y, room);
    }
    return 0;
}

void LoadWorld(const char *world_name) {
    ldtk::Project ldtk_project;
    ldtk_project.loadFromFile("assets/world/game_world.ldtk");
    const auto &world = ldtk_project.getWorld(world_name);
    for (const ldtk::Level &level: world.allLevels()) {
        const ldtk::Layer &collision_layer = level.getLayer("collisions");

        Room *room = arraddnptr(state.rooms, 1);
        memset(room, 0, sizeof(*room));

        room->offset_x = level.position.x / 32;
        room->offset_y = level.position.y / 32;
        room->width = level.size.x / 32;
        room->height = level.size.y / 32;

        for (i32 x = 0; x < room->width; ++x) {
            for (i32 y = 0; y < room->height; ++y) {
                const ldtk::IntGridValue &grid_val = collision_layer.getIntGridVal(x, y);
                room->tiles[x + y * room->width] = grid_val.value > 0;
            }
        }
        for (const ldtk::Layer &layer: level.allLayers()) {
            if (layer.getType() == ldtk::LayerType::AutoLayer) {
                for (const auto &tile: layer.allTiles()) {
                    const auto &position = tile.getWorldPosition();
                    const auto &texture_rect = tile.getTextureRect();

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

        for (const ldtk::Layer &layer: level.allLayers()) {
            if (layer.getType() != ldtk::LayerType::Entities) continue;

            for (auto &data_entity: layer.allEntities()) {
                Entity *entity = NULL;
                if (data_entity.getName() == "player" && PLAYER) {
                    PLAYER->position = Vector2{
                        (f32) data_entity.getWorldPosition().x / 32, (f32) data_entity.getWorldPosition().y / 32
                    };
                    continue;
                }

                if (data_entity.getName() == "treasure") {
                    entity = new Treasure();
                }

                if (data_entity.getName() == "guard") {
                    entity = new Guard();
                }

                if (entity) {
                    entity->position = Vector2{
                        (f32) data_entity.getWorldPosition().x / 32, (f32) data_entity.getWorldPosition().y / 32
                    };
                    entity->Configure(world, data_entity);
                    AddEntity(entity);
                }
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

static void StartLevel() {
    // player
    Player *player = new Player();
    player->position = {2, 2};
    AddEntity(player);
    LoadWorld("tutorial");
}

static void GameSetup()
{
    memset(&state, 0, sizeof(GameState));

    state.camera.target = Vector2{ 0, 0 };
    state.camera.offset = Vector2{ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    state.camera.rotation = 0.0f;
    state.camera.zoom = 2.5f;

    StartLevel();
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
    arrfree(state.rooms);
}

f32 GameRaycast(Vector2 pos, Vector2 dir)
{
    f32 min_t = 1e30;

    for (u32 i = 0; i < arrlen(state.rooms); ++i)
    {
        Room *room = state.rooms + i;
        for (u32 dy = 0; dy < room->height; ++dy)
        {
            for (u32 dx = 0; dx < room->width; ++dx)
            {
                i32 x = room->offset_x + dx;
                i32 y = room->offset_y + dy;

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
    }

    return min_t;
}

void UpdateCamera(const Entity *entity, f32 delta)
{
    Vector2 playerPos = entity->position;
    Vector2 cameraPos = state.camera.target;

    // Smoothing camera following player
    float targetX = 0.1f * (playerPos.x + 0.5) * 32 + 0.9f * cameraPos.x;
    float targetY = 0.1f * (playerPos.y + 0.5) * 32 + 0.9f * cameraPos.y;
    state.camera.target = Vector2(targetX, targetY);
}

static void GameDrawCone(Vector2 pos, Vector2 dir, f32 length, f32 angle)
{
    u32 sample_points = 128;

    f32 forward_angle = 0;
    f32 start_angle = forward_angle + angle / 2;
    f32 end_angle = forward_angle - angle / 2;

    Vector2 prev_sample;

    {
        Vector2 current_dir = {cos(start_angle / 180.0f * PI), sin(start_angle / 180.0f * PI)};
        f32 len = Min(GameRaycast(pos, current_dir), length);
        prev_sample = (pos + current_dir * len) * 32;
    }

    for (u32 i = 1; i < sample_points; ++i)
    {
        f32 current_angle = start_angle + (end_angle - start_angle) * ((f32) i / ((f32) sample_points - 1));
        Vector2 current_dir = {cos(current_angle / 180.0f * PI), sin(current_angle / 180.0f * PI)};

        f32 len = Min(GameRaycast(pos, current_dir), length);
        Vector2 sample = (pos + current_dir * len) * 32;

        DrawTriangle(pos * 32, prev_sample, sample, Fade(BLUE, 0.5));
        prev_sample = sample;
    }
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
    ClearBackground({34, 32, 52, 255});
    BeginMode2D(state.camera);

    for (u32 i = 0; i < arrlen(state.textured_tiles); ++i)
    {
        TexturedTile textured = state.textured_tiles[i];
        DrawTextureRec(tileset, textured.source, textured.position, WHITE);
    }

    // Draw colliders
    for (u32 i = 0; i < arrlen(state.rooms); ++i)
    {
        Room *room = state.rooms + i;
        for (u32 dx = 0; dx < room->width; ++dx)
        {
            for (u32 dy = 0; dy < room->height; ++dy)
            {
                i32 x = room->offset_x + dx;
                i32 y = room->offset_y + dy;
                u8 tile = GetTile(x, y);

                // if (tile)
                //     DrawRectangle(x * 32, y * 32, 32, 32, BLACK);
            }
        }
    }

    for (u32 i = 0; i < arrlen(state.entities); ++i)
    {
        Entity *entity = state.entities[i];
        entity->Draw();
    }

    if (PLAYER != NULL)
        GameDrawCone(PLAYER->position, {1, 0}, 6, 45);

    EndMode2D();

    {
        // fade in
        f32 t = Range(game_scene->time, 0, 0.65);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 1 - t));
    }

    DrawFPS(10, 10);

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
