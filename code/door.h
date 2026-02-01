#pragma once
#include "entity.h"


struct Door : Entity {
    bool unlocked = false;
    bool unlockable;

    float animation_timer = -1;
    u32 unlock_step = 0;
    f32 unlock_offset = 0;
    f32 unlock_range = 0;
    f32 unlock_cursor = 0;
    f32 unlock_cursor_speed = 0;
    f32 unlock_error_timer = 0;

    void Update(f32 delta) override;
    void PreDraw() override;
    void PostDraw() override;

    void Open();

    void Configure(const ldtk::World &world, Room* room, const ldtk::Entity &data) override;

    void SetActiveInteraction(bool active) override;

    void SelectRandomUnlockRange();
};