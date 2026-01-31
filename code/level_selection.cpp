#include "level_selection.h"

#include "raylib.h"

Scene *level_selection = {};

struct LevelSelectionState {
    u32 target_level;
    f32 current_level;
    RenderTexture2D target;
};

LevelSelection levels[LEVEL_COUNT] = {};

static LevelSelectionState menu_state;
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

    BeginTextureMode(menu_state.target);

    for (int i = 0; i < LEVEL_COUNT; ++i) {

        f32 offset = i * 300;
        LevelSelection *level = &levels[i];
        DrawTexture(preview_frame, 249 + offset, 233, WHITE);
        DrawTexture(level->preview, 254 + offset, 238, WHITE);
    }

    EndTextureMode();

    BeginDrawing();

    const f32 ratio = MENU_WIDTH / (f32) MENU_HEIGHT;

    // TODO: scale up
    DrawTexturePro(menu_state.target.texture, {0,0, MENU_WIDTH, MENU_HEIGHT}, {0,0, MENU_WIDTH, MENU_HEIGHT}, {}, 0, WHITE);

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