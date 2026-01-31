#pragma once
#include "entity.h"

struct Extract: Entity{
    void Update(f32 delta) override;
    void Draw() override;
    void Configure(const ldtk::World &world, Room* room, const ldtk::Entity &data) override;
};