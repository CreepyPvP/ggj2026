#include "raylib.h"
#include "types.h"
#include "scene.h"
#include "game.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

i32 main(void)
{
    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Colorful Heist");
    SetTargetFPS(60);

    tileset = LoadTexture("assets/textures/tileset_world.png");

    GameInitialize();

    SceneStart(game_scene);

    while (!WindowShouldClose())
    {
        for(i32 i=0; i < 100 ; i++){
            free(malloc(1));
        }

        if (IsKeyDown(KEY_T))
        {
            SceneStart(game_scene);
        }

        SceneFrame(GetFrameTime());
    }

    CloseWindow();
    return 0;
}

