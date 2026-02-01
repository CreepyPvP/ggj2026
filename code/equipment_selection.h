#pragma once
#include "game.h"
#include "raylib.h"
#include "scene.h"

extern Scene* equipment_selection;

struct EquipmentSelection {

    int max_coins;
    int coin_amount;

    Equipment equipment;

    RenderTexture target;
};

extern EquipmentSelection eqs_state;

void InitializeEquipmentSelection();