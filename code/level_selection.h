#pragma once
#include "raylib.h"
#include "scene.h"

#define LEVEL_COUNT 3

extern Scene *level_selection;

struct LevelSelection {
    Texture preview;
    const char *name;
    const char *world_id;
    f32 game_time;
};

struct LevelSelectionState {
    u32 target_level;
    f32 current_level;
    RenderTexture2D target;
};


extern LevelSelectionState menu_state;

extern LevelSelection levels[LEVEL_COUNT];

void InitializeLevelSelection();