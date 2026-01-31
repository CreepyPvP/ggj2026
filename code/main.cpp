#include "raylib.h"
#include "types.h"
#include "scene.h"
#include "game.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

i32 main(void)
{
    InitWindow(1600, 900, "Colorful Heist");
    SetTargetFPS(60);

    tileset = LoadTexture("assets/textures/tileset_world.png");

    GameInitialize();

    SceneStart(game_scene);

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
