#include "raylib.h"
#include "types.h"
#include "scene.h"
#include "game.h"

i32 main(void)
{
    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Colorful Heist");
    SetTargetFPS(60);

    GameInitialize();

    SceneStart(game_scene);

    while (!WindowShouldClose())
    {
        SceneFrame(GetFrameTime());
    }

    CloseWindow();
    return 0;
}
