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
    SetExitKey(0);

    SetTargetFPS(60);

    tileset = LoadTexture("assets/textures/tileset_world.png");

    LoadScores();

    GameInitialize();
    InitializeLevelSelection();

    SceneStart(level_selection);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_T) && IsKeyDown(KEY_LEFT_CONTROL))
        {
            SceneStart(level_selection);
        }

        SceneFrame(GetFrameTime());
    }

    CloseWindow();
    return 0;
}

