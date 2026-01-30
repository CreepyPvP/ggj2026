#include "game.h"
#include "scene.h"
#include "game_math.h"

#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "LDtkLoader/Project.hpp"

Scene *game_scene;

static void GameSetup()
{
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

static void GameFrame(f32 delta)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Hello world from game", 190, 200, 20, LIGHTGRAY);

    {
        // fade in
        f32 t = Range(game_scene->time, 0, 0.8);
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
}
