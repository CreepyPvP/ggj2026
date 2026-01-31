#include "guard.h"

#include "game.h"
#include "entity.h"
#include "raymath.h"
#include "LDtkLoader/World.hpp"

void Guard::Update(f32 delta) {
    Entity::Update(delta);

    // Update cone
}

void Guard::Draw() {
    Entity::Draw();

    Vector2 render_pos = {floorf(this->position.x * 32), floorf(this->position.y * 32)};
    DrawTextureRec(tileset, Rectangle{32, 448, 32, 32}, render_pos, {255,255,255,255});
}

void Guard::Configure(const ldtk::World &world, const ldtk::Entity &data) {
    Entity::Configure(world, data);

    // Loading guard paths
    auto loadedPaths = data.getArrayField<ldtk::IntPoint>("PatrolPath");
    PatrolPathSize =  loadedPaths.size();
    int i = 0;
    for (auto& path : loadedPaths) {
        patrolPath[i] = Vector2{
            (f32) path.value().x / 32, (f32) path.value().y / 32
        };
        i++;
    }

    // Loading guards color
    const auto &enum_value = data.getField<ldtk::EnumValue>("color");
    if (enum_value == world.getEnum("color")["red"]) color = Red;
    if (enum_value == world.getEnum("color")["green"]) color = Green;
    if (enum_value == world.getEnum("color")["blue"]) color = Blue;
}
