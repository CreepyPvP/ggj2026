#pragma once
#include "entity.h"


enum TreasureType {
    money_big,
    coins_big,
    diamond_big,
    money_small,
    coins_small,
    diamond_small
};

struct Treasure: Entity{
    bool collected = false;
    TreasureType type;
    void Update(f32 delta) override;
    void Draw() override;
    void Configure(const ldtk::World &world, const ldtk::Entity &data) override;
};