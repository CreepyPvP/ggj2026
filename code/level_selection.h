#pragma once
#include "raylib.h"
#include "scene.h"

#define LEVEL_COUNT 2

extern Scene *level_selection;

struct LevelSelection {
    Texture preview;
    const char *name;
    const char *world_id;
};

extern LevelSelection levels[LEVEL_COUNT];

void InitializeLevelSelection();