#include "raylib.h"
#include "types.h"
#include "scene.h"
#include "game.h"

#include "score.h"

#define STB_DS_IMPLEMENTATION
#include "equipment_selection.h"
#include "level_selection.h"
#include "stb_ds.h"

i32 main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1600, 900, "Colorful Heist");
    SetExitKey(0);
    InitAudioDevice();  

    SetTargetFPS(60);

    tileset = LoadTexture("assets/textures/tileset_world.png");

    guardSound1 = LoadSound("assets/sounds/GuardSound1.wav");
    guardSound2 = LoadSound("assets/sounds/GuardSound2.wav");
    coinSound = LoadSound("assets/sounds/coingain.wav");
    switchSound = LoadSound("assets/sounds/switch.wav");
    slideSound = LoadSound("assets/sounds/slide.wav");
    failureSound = LoadSound("assets/sounds/failurebell.wav");
    stepSound = LoadSound("assets/sounds/step.wav");

    LoadScores();

    GameInitialize();
    InitializeLevelSelection();
    InitializeEquipmentSelection();

    SceneStart(level_selection);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_T) && IsKeyDown(KEY_LEFT_CONTROL))
        {
            SceneStart(level_selection);
        }

        SceneFrame(GetFrameTime());
    }

    CloseAudioDevice();
    CloseWindow();

    return 0;
}

