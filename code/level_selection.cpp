#include "level_selection.h"

#include "equipment_selection.h"
#include "game.h"
#include "game_math.h"
#include "raylib.h"
#include "raymath.h"
#include "score.h"

Scene *level_selection = {};
LevelSelection levels[LEVEL_COUNT] = {};
static u32 last_level = 0;
LevelSelectionState menu_state;

#define MENU_WIDTH 640
#define MENU_HEIGHT 448

static Texture preview_frame;

static void LevelSelectionSetup() {
    score_manager.show_screen = false;
    menu_state = {};
    menu_state.target = LoadRenderTexture(MENU_WIDTH, MENU_HEIGHT);
    menu_state.current_level = last_level;
    menu_state.target_level = last_level;
}

static void LevelSelectionFrame(f32 delta) {
    if (!score_manager.show_screen) {
        if ((IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) && menu_state.target_level > 0) {
            menu_state.target_level--;
        }
        if ((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) && menu_state.target_level < LEVEL_COUNT - 1) {
            menu_state.target_level++;
        }
        if (IsKeyPressed(KEY_SPACE)) {
            SceneStart(game_scene);
        }
    }
    if (IsKeyPressed(KEY_TAB)) {
        ToggleList(menu_state.target_level);
    }

    menu_state.current_level = Vector2MoveTowards({menu_state.current_level, 0}, {(f32) menu_state.target_level, 0},
                                                  delta * 4).x;

    BeginTextureMode(menu_state.target);
    ClearBackground(BLACK);

    for (int i = 0; i < LEVEL_COUNT; ++i) {
        f32 distance = i - menu_state.current_level;
        f32 abs_distance = fabsf(distance);
        f32 offset = distance < 0
                         ? distance * (130 - (abs_distance - 1) * 20)
                         : distance * (150 - (abs_distance - 1) * 17);
        LevelSelection *level = &levels[i];

        DrawTextureEx(preview_frame, {249 + offset, 203 + 10 * abs_distance}, 0, Max(1 - 0.2f * abs_distance, 0),
                      ColorAlpha(WHITE, Max(1 - 0.45f * abs_distance, 0)));
        DrawTextureEx(level->preview, {254 + offset, 208 + 10 * abs_distance}, 0, Max(1 - 0.2f * abs_distance, 0),
                      ColorAlpha(WHITE, Max(1 - 0.45f * abs_distance, 0)));
    }

    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);

    const f32 ratio = MENU_WIDTH / (f32) MENU_HEIGHT;

    const f32 new_width = GetScreenHeight() * ratio;
    const f32 new_height = GetScreenHeight();

    DrawTexturePro(menu_state.target.texture, {0, 0, MENU_WIDTH, -MENU_HEIGHT}, Rectangle{
                       GetScreenWidth() / 2 - new_width / 2, GetScreenHeight() / 2 - new_height / 2, new_width,
                       new_height
                   }, {}, 0, WHITE);

    for (int i = 0; i < LEVEL_COUNT; ++i) {
        LevelSelection *level = &levels[i];
        f32 distance = Min(fabsf(i - menu_state.current_level), 1);

        i32 width = MeasureText(level->name, 50);
        DrawText(level->name, GetScreenWidth() / 2 - width / 2, GetScreenHeight() / 2 - GetScreenHeight() * 0.3f, 50,
                 ColorAlpha(WHITE, 1 - distance));
    }

    static const char *instructions = "Press 'SPACE' to select";
    i32 width = MeasureText(instructions, 50);
    DrawText(instructions, GetScreenWidth() / 2 - width / 2, GetScreenHeight() - GetScreenHeight() * 0.1f, 50, WHITE);

    DrawScoreScreen();

    EndDrawing();
}

static void LevelSelectionDestroy() {
    last_level = menu_state.target_level;
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
        .world_id = "tutorial",
        .game_time = 183,
    };
    levels[1] = {
        .preview = LoadTexture("assets/textures/levels/level_preview.png"),
        .name = "The Bank Job",
        .world_id = "level",
        .game_time = 183,
    };
    levels[2] = {
        .preview = LoadTexture("assets/textures/levels/level2_preview.png"),
        .name = "A visit to the museum",
        .world_id = "society",
        .game_time = 183,
    };
}
