#pragma once
#include "entity.h"

struct Treasure: Entity{
    bool collected = false;
    void Update(f32 delta) override;
    void Draw() override;
};