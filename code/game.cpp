#include "game.h"

#include <stdlib.h>
#include <string.h>

#include "door.h"
#include "entity.h"
#include "extract.h"
#include "scene.h"
#include "game_math.h"
#include "guard.h"
#include "level_selection.h"
#include "switch.h"
#include "player.h"

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include "treasure.h"

#include "LDtkLoader/Project.hpp"

Scene *game_scene;
Texture2D tileset;

RenderTexture2D render_target;
RenderTexture2D light_target;

Shader light_shader;
i32 light_shader_color_buffer_loc;
i32 light_shader_light_buffer_loc;
i32 light_shader_size_loc;

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

void SetTile(i32 int_x, i32 int_y, Room* room, int value){
    
    i32 rx = int_x - room->offset_x;
    i32 ry = int_y - room->offset_y;
    if (rx < 0 || ry < 0 || rx >= room->width || ry >= room->height){
        printf("Invalid SetTile call!!!");
        return;
    }
    room->tiles[rx + ry * room->width] = value;
}

void SetTile(float x, float y, int value){
    i32 int_x = (int)x;
    i32 int_y = (int)y;
    
    for (u32 i = 0; i < arrlen(state.rooms); ++i)
    {
        Room *room = state.rooms + i;
        if(room->offset_x <= int_x && room->offset_x + room->width >= int_x && 
        room->offset_y <= int_y && room->offset_y + room->width >= int_y){
            SetTile(int_x,int_y, room, value);
        }
        
    }

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
                if(data_entity.getName() == "camera"){
                    entity = new GuardCamera();
                }
                if (data_entity.getName() == "door") {
                    entity = new Door();
                    room->tiles[data_entity.getPosition().x / 32 + data_entity.getPosition().y / 32 * room->width] = 1;
                }

                if (data_entity.getName() == "extract") {
                    entity = new Extract();
                }

                if (entity) {
                    entity->position = Vector2{
                        (f32) data_entity.getWorldPosition().x / 32, (f32) data_entity.getWorldPosition().y / 32
                    };
                    entity->ldtk_id = data_entity.iid;
                    entity->Configure(world, room, data_entity);
                    AddEntity(entity);
                }
            }
            for (auto &data_entity: layer.allEntities()) {
                Entity *entity = NULL;
                if (data_entity.getName() == "switch") {
                    entity = new Switch();
                }
                if (entity) {
                    entity->position = Vector2{
                        (f32) data_entity.getWorldPosition().x / 32, (f32) data_entity.getWorldPosition().y / 32
                    };
                    entity->ldtk_id = data_entity.iid;
                    entity->Configure(world, room, data_entity);
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



Entity* getEntity(ldtk::IID ldtk_id){
    for (u32 i = 0; i < arrlen(state.entities); ++i){
        if(state.entities[i]->ldtk_id == ldtk_id){
            return state.entities[i];
        }
    }
    return NULL;
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
    LoadWorld(levels[menu_state.target_level].world_id);
}

static void GameSetup()
{
    memset(&state, 0, sizeof(GameState));

    state.camera.target = Vector2{ 0, 0 };
    state.camera.offset = Vector2{ (f32)render_target.texture.width/2.0f, (f32) render_target.texture.height / 2.0f };
    state.camera.rotation = 0.0f;
    state.camera.zoom = 2.5f;

    state.game_countdown = 300.0f;

    StartLevel();
    printf("Game setup\n");
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

f32 GameRaycast(Vector2 pos, Vector2 dir, f32 max_dist)
{
    f32 min_t = 1e30;

    i32 tile_x = pos.x;
    i32 tile_y = pos.y;
    i32 sign_x = dir.x >= 0 ? 1 : -1;
    i32 sign_y = dir.y >= 0 ? 1 : -1;

    u32 steps = max_dist + 1;

    for (u32 dy = 0; dy < steps; ++dy)
    {
        for (u32 dx = 0; dx < steps; ++dx)
        {
            i32 x = tile_x + dx * sign_x;
            i32 y = tile_y + dy * sign_y;

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
    float targetX = 0.1f * (playerPos.x + 0.5) * 32 + 0.9f * cameraPos.x;
    float targetY = 0.1f * (playerPos.y + 0.5) * 32 + 0.9f * cameraPos.y;
    state.camera.target = Vector2(targetX, targetY);
}

struct ConeDraw
{
    Vector2 pos;
    f32 forward_angle;
    f32 length;
    f32 angle;
    Color color;
};

static ConeDraw *cone_draws;

void GameDrawCone(Vector2 pos, f32 forward_angle, f32 length, f32 angle, Color color)
{
    ConeDraw draw = {};
    draw.pos = pos;
    draw.forward_angle = forward_angle;
    draw.length = length;
    draw.angle = angle;
    draw.color = color;

    if (PLAYER)
    {
        if (Vector2Length(pos -  PLAYER->position) > 20 + length)
        {
            return;
        }
    }

    arrput(cone_draws, draw);
}

static void ExecuteConeDraw(ConeDraw *draw)
{
    u32 sample_points = 64;

    f32 start_angle = draw->forward_angle + draw->angle / 2;
    f32 end_angle = draw->forward_angle - draw->angle / 2;

    Vector2 prev_sample;

    {
        Vector2 current_dir = {cos(start_angle / 180.0f * PI), sin(start_angle / 180.0f * PI)};
        f32 len = Min(GameRaycast(draw->pos, current_dir, draw->length + 2), draw->length);
        prev_sample = (draw->pos + current_dir * len) * 32;
    }

    for (u32 i = 1; i < sample_points; ++i)
    {
        f32 current_angle = start_angle + (end_angle - start_angle) * ((f32) i / ((f32) sample_points - 1));
        Vector2 current_dir = {cos(current_angle / 180.0f * PI), sin(current_angle / 180.0f * PI)};

        f32 len = Min(GameRaycast(draw->pos, current_dir, draw->length + 2), draw->length);
        Vector2 sample = (draw->pos + current_dir * len) * 32;

        DrawTriangle(draw->pos * 32, prev_sample, sample, Fade(draw->color, 0.5));
        prev_sample = sample;
    }
}

void GameStartLose()
{
    state.game_lost = true;
}

static void GameFrame(f32 delta)
{
    if (state.game_lost)
    {
        f32 t = Range(state.time_since_game_lost, 0, 1.1);
        state.time_since_game_lost += delta;
        delta = Lerp(0.002, 0.0007, t);
        if (state.time_since_game_lost > 1.3)
        {
            SceneStart(game_scene);
            return;
        }
    }

    if (delta > 0.03)
    {
        delta = 0.03;
    }

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

    BeginTextureMode(render_target);

    ClearBackground({26, 24, 39, 255});
    BeginMode2D(state.camera);

    for (u32 i = 0; i < arrlen(state.textured_tiles); ++i)
    {
        TexturedTile textured = state.textured_tiles[i];
        DrawTextureRec(tileset, textured.source, textured.position, WHITE);
    }

    // Draw colliders
    // for (u32 i = 0; i < arrlen(state.rooms); ++i)
    // {
    //     Room *room = state.rooms + i;
    //     for (u32 dx = 0; dx < room->width; ++dx)
    //     {
    //         for (u32 dy = 0; dy < room->height; ++dy)
    //         {
    //             i32 x = room->offset_x + dx;
    //             i32 y = room->offset_y + dy;
    //             u8 tile = GetTile(x, y);
    //
    //             if (tile)
    //                 DrawRectangle(x * 32, y * 32, 32, 32, BLACK);
    //         }
    //     }
    // }

    arrsetlen(cone_draws, 0);
    for (u32 i = 0; i < arrlen(state.entities); ++i)
    {
        Entity *entity = state.entities[i];
        entity->PreDraw();
    }
    for (u32 i = 0; i < arrlen(state.entities); ++i)
    {
        Entity *entity = state.entities[i];
        entity->Draw();
    }

    EndMode2D();
    EndTextureMode();

    BeginTextureMode(light_target);
    ClearBackground({0, 0, 0, 255});
    Camera2D light_camera = state.camera;
    light_camera.offset = Vector2{ (f32)light_target.texture.width/2.0f, (f32) light_target.texture.height / 2.0f };
    light_camera.zoom *= 0.50 * 0.6;
    BeginMode2D(light_camera);
    for (u32 i = 0; i < arrlen(cone_draws); ++i)
    {
        ExecuteConeDraw(cone_draws + i);
    }
    EndMode2D();
    EndTextureMode();

    BeginDrawing();


    // Final composite pass
    BeginShaderMode(light_shader);
    Vector2 screen_size = { (f32) GetScreenWidth(), (f32) GetScreenHeight() };
    SetShaderValueTexture(light_shader, light_shader_color_buffer_loc, render_target.texture);
    SetShaderValueTexture(light_shader, light_shader_light_buffer_loc, light_target.texture);
    SetShaderValue(light_shader, light_shader_size_loc, &screen_size, RL_SHADER_UNIFORM_VEC2);
    // DrawTexturePro(render_target.texture, {0, 0, (f32) render_target.texture.width, (f32) -render_target.texture.height},
    //                {0, 0, (f32) GetScreenWidth(), (f32) GetScreenHeight()}, {}, 0, WHITE);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);
    EndShaderMode();

    Camera2D composite_camera = state.camera;
    composite_camera.offset = Vector2{ (f32)GetScreenWidth()/2.0f, (f32) GetScreenHeight() / 2.0f };
    composite_camera.zoom *= (f32) GetScreenWidth() / (f32) render_target.texture.width;
    BeginMode2D(composite_camera);

    for (u32 i = 0; i < arrlen(state.entities); ++i)
    {
        Entity *entity = state.entities[i];
        entity->PostDraw();
    }

    EndMode2D();


    // Drawing DeathScreen
    if (state.game_lost) {
        unsigned char alpha = 255 * (state.time_since_game_lost / 0.5);
        i32 fontSize = 200.0f * (state.time_since_game_lost / 1.3);
        DrawText("BUSTED", 1600.0f*0.27f,900.0f*0.35f,fontSize, { 230, 41, 55, alpha});
    }

    int fontSize = 45;
    DrawRectangle(0,0, 550, 180, ColorAlpha(BLACK, 0.5));
    DrawText(TextFormat("Total cash:\t%dk", state.saved_cash),0.5 * fontSize,2 * fontSize, fontSize, WHITE);
    DrawText(TextFormat("Held cash:  \t%dk", state.held_cash),0.5 * fontSize,3 * fontSize, fontSize, WHITE);

    {
        // fade in
        f32 t = Range(game_scene->time, 0, 1.25);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 1 - t));
    }

    state.game_countdown -= delta;
    if (state.game_countdown < 0) state.game_countdown = 0;
    int mins = state.game_countdown / 60;
    int seconds = state.game_countdown - 60 * mins;
    int millisec = (state.game_countdown - 60 * mins - seconds) * 1000;
    DrawText(TextFormat("Countdown:\t%d:%d:%d", mins, seconds, millisec), 0.5 * fontSize,1* fontSize, fontSize, WHITE);

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

    render_target = LoadRenderTexture(1600, 900);
    light_target = LoadRenderTexture(480, 270);

    light_shader = LoadShader(NULL, "assets/lightshader.frag");
    light_shader_color_buffer_loc = GetShaderLocation(light_shader, "color_buffer");
    light_shader_light_buffer_loc = GetShaderLocation(light_shader, "light_buffer");
    light_shader_size_loc = GetShaderLocation(light_shader, "size");
}
