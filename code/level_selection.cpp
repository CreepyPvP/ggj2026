#include "level_selection.h"

#include "game.h"
#include "game_math.h"
#include "raylib.h"
#include "raymath.h"

Scene *level_selection = {};
LevelSelection levels[LEVEL_COUNT] = {};

LevelSelectionState menu_state;

#define MENU_WIDTH 640
#define MENU_HEIGHT 448

static Texture preview_frame;

static void LevelSelectionSetup() {

    menu_state = {};
    menu_state.target = LoadRenderTexture(MENU_WIDTH, MENU_HEIGHT);
}

static void LevelSelectionFrame(f32 delta) {

    if (IsKeyPressed(KEY_A) && menu_state.target_level > 0) menu_state.target_level--;
    if (IsKeyPressed(KEY_D) && menu_state.target_level < LEVEL_COUNT-1) menu_state.target_level++;
    if (IsKeyPressed(KEY_SPACE)) {
        SceneStart(game_scene);
    }

    menu_state.current_level = Vector2MoveTowards({menu_state.current_level, 0}, {(f32) menu_state.target_level, 0}, delta * 4).x;

    BeginTextureMode(menu_state.target);
    ClearBackground(BLACK);

    for (int i = 0; i < LEVEL_COUNT; ++i) {

        f32 distance = i - menu_state.current_level;
        f32 abs_distance = fabsf(distance);
        f32 offset = distance < 0 ? distance * (130 - (abs_distance-1) * 20) : distance * (150 - (abs_distance-1) * 17);
        LevelSelection *level = &levels[i];

        DrawTextureEx(preview_frame, {249 + offset, 203 + 10 * abs_distance}, 0, Max(1 - 0.2f * abs_distance, 0), ColorAlpha(WHITE, Max(1 - 0.45f * abs_distance, 0)));
        DrawTextureEx(level->preview,{ 254 + offset, 208 + 10 * abs_distance}, 0 , Max(1 - 0.2f * abs_distance, 0), ColorAlpha(WHITE, Max(1 - 0.45f * abs_distance, 0)));
    }

    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);

    const f32 ratio = MENU_WIDTH / (f32) MENU_HEIGHT;

    // TODO: scale up
    const f32 new_width = GetScreenHeight() * ratio;
    const f32 new_height = GetScreenHeight();

    DrawTexturePro(menu_state.target.texture, {0,0, MENU_WIDTH, -MENU_HEIGHT}, Rectangle{GetScreenWidth() / 2 - new_width/2,GetScreenHeight() / 2 - new_height/2, new_width, new_height}, {}, 0, WHITE);

    for (int i = 0; i < LEVEL_COUNT; ++i) {
        LevelSelection *level = &levels[i];
        f32 distance = Min(fabsf(i - menu_state.current_level), 1);

        i32 width = MeasureText(level->name, 50);
        DrawText(level->name, GetScreenWidth() / 2 - width / 2, GetScreenHeight() / 2 - GetScreenHeight() * 0.3f, 50, ColorAlpha(WHITE, 1 - distance));
    }

    static const char *instructions = "Press 'SPACE' to select";
    i32 width = MeasureText(instructions, 50);
    DrawText(instructions, GetScreenWidth() / 2 - width / 2, GetScreenHeight() - GetScreenHeight() * 0.1f, 50, WHITE);


    EndDrawing();
}

static void LevelSelectionDestroy() {
    UnloadRenderTexture(menu_state.target);
}


void InitializeLevelSelection() {

    level_selection = (Scene *) malloc(sizeof(Scene));
    memset(level_selection, 0, sizeof(*level_selection));
    level_selection->Setup = LevelSelectionSetup;
    level_selection->Frame = LevelSelectionFrame;
    level_selection->Destroy = LevelSelectionDestroy;
    preview_frame = LoadTexture("assets/textures/levels/preview_frame.png");
    levels[0] = {
        .preview = LoadTexture("assets/textures/levels/tutorial_preview.png"),
        .name = "Tutorial",
        .world_id = "tutorial"
    };
    levels[1] = {
        .preview = LoadTexture("assets/textures/levels/level_preview.png"),
        .name = "Game Level 1",
        .world_id = "level"
    };
}