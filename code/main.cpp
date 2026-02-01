#include "raylib.h"
#include "types.h"
#include "scene.h"
#include "game.h"

#include "score.h"

#define STB_DS_IMPLEMENTATION
#include "level_selection.h"
#include "stb_ds.h"

i32 main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1600, 900, "Colorful Heist");
    InitAudioDevice();  

    SetTargetFPS(60);

    tileset = LoadTexture("assets/textures/tileset_world.png");

    guardSound1 = LoadSound("assets/sounds/GuardSound1.wav");
    guardSound2 = LoadSound("assets/sounds/GuardSound2.wav");
    LoadScores();

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

    CloseAudioDevice();
    CloseWindow();

    return 0;
}

