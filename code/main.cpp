#include "raylib.h"
#include "types.h"
#include "scene.h"
#include "game.h"

#define STB_DS_IMPLEMENTATION
#include "level_selection.h"
#include "stb_ds.h"

i32 main(void)
{
    InitWindow(1600, 900, "Colorful Heist");
    SetTargetFPS(60);

    tileset = LoadTexture("assets/textures/tileset_world.png");

    GameInitialize();
    InitializeLevelSelection();

    SceneStart(level_selection);

    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_T))
        {
            SceneStart(game_scene);
        }

        SceneFrame(GetFrameTime());
    }

    CloseWindow();
    return 0;
}

