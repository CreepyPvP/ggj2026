#pragma once
#include "entity.h"


struct Door : Entity {
    bool unlocked = false;
    bool unlockable;

    float animation_timer = -1;

    void Update(f32 delta) override;
    void PreDraw() override;

    void Open();

    void Configure(const ldtk::World &world, Room* room, const ldtk::Entity &data) override;

};