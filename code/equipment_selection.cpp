#include "equipment_selection.h"

#include "game.h"
#include "game_math.h"
#include "raylib.h"
#include "raymath.h"

Scene* equipment_selection{};
EquipmentSelection eqs_state;


static Texture level_blueprint;
static Texture maskRed;
static Texture maskGreen;
static Texture maskBlue;
static Texture coin;
static Texture coinEmpty;
static Texture frameSmall;
static Texture frameBig;
static Texture dietrich;


#define MENU_WIDTH 640
#define MENU_HEIGHT 448

static void EquipmentSelectionSetup() {
    eqs_state = {};
    
    eqs_state.target = LoadRenderTexture(MENU_WIDTH, MENU_HEIGHT);

    //eqs_state.level_blueprint = LoadTexture("assets");
}

static void EquipmentSelectionFrame(f32 delta) {

    Vector2 mousePos = GetMousePosition();

    if (IsKeyPressed(KEY_SPACE)) {
        if (!eqs_state.equipment.mask_blue || !eqs_state.equipment.mask_red || !eqs_state.equipment.mask_green) {
            return; // TODO: remove this shit
        }

        SceneStart(game_scene);
    }


    BeginTextureMode(eqs_state.target);
    ClearBackground(BLACK);

    Vector2 pivot = {MENU_WIDTH / 2.0f, MENU_HEIGHT / 2.0f};


    for (int i = 0; i < 3; ++i) {
        Vector2 pos = {pivot.x + (frameSmall.width + 10) * i, pivot.y};
        DrawTexture(frameSmall, pos.x, pos.y, WHITE);
        DrawTexture(frameSmall, pos.x, pos.y, WHITE);
    }

    EndTextureMode();


    BeginDrawing();
    ClearBackground(BLACK);

    const f32 ratio = MENU_WIDTH / (f32) MENU_HEIGHT;

    const f32 new_width = GetScreenHeight() * ratio;
    const f32 new_height = GetScreenHeight();

    DrawTexturePro(eqs_state.target.texture, {0, 0, MENU_WIDTH, -MENU_HEIGHT}, Rectangle{
                       GetScreenWidth() / 2 - new_width / 2, GetScreenHeight() / 2 - new_height / 2, new_width,
                       new_height
                   }, {}, 0, WHITE);
    EndDrawing();
}

static void EquipmentSelectionDestroy() {
    UnloadRenderTexture(eqs_state.target);
}



void InitializeEquipmentSelection() {
    equipment_selection = (Scene *) malloc(sizeof(Scene));
    memset(equipment_selection, 0, sizeof(*equipment_selection));
    equipment_selection->Setup = EquipmentSelectionSetup;
    equipment_selection->Frame = EquipmentSelectionFrame;
    equipment_selection->Destroy = EquipmentSelectionDestroy;
    
    // Loading mask textures
    maskRed = LoadTexture("assets/textures/redmask.png");
    maskGreen = LoadTexture("assets/textures/greenmask.png");
    maskBlue = LoadTexture("assets/textures/bluemask.png");

    coin = LoadTexture("assets/textures/coin.png");
    coinEmpty = LoadTexture("assets/textures/coinEmpty.png");
    dietrich = LoadTexture("assets/textures/dietrich.png");
    frameSmall = LoadTexture("assets/textures/frameSmall.png");
    frameBig = LoadTexture("assets/textures/frameBig.png");
}