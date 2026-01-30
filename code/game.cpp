#include "game.h"
#include "scene.h"
#include "game_math.h"

#include <stdlib.h>
#include <string.h>
#include "raylib.h"

Scene *game_scene;


static void GameSetup()
{
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
