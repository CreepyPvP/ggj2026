#include "guard.h"

#include "game.h"
#include "entity.h"
#include "raymath.h"
#include "LDtkLoader/World.hpp"

void Guard::Update(f32 delta) {
    Entity::Update(delta);


    Vector2 nextPoint = patrolPath[NextPatrolPoint];
    Vector2 direction = Vector2Scale(Vector2Normalize((nextPoint - position)), speed*delta);
    position = direction + position;

    if (Vector2DistanceSqr(position + direction, nextPoint ) > Vector2DistanceSqr(position,nextPoint)) {
        NextPatrolPoint = (NextPatrolPoint + 1) % PatrolPathSize;
    }

    ConeRotation += rotationSpeed* delta;
}

void Guard::Draw() {
    Entity::Draw();

    Vector2 render_pos = {floorf(this->position.x * 32), floorf(this->position.y * 32)};
    DrawTextureRec(tileset, Rectangle{32, 448, 32, 32}, render_pos, {255,255,255,255});

    GameDrawCone({position.x+0.5f,position.y+0.5f},ConeRotation, 10, 45);
}

void Guard::Configure(const ldtk::World &world, Room* room, const ldtk::Entity &data) {
    Entity::Configure(world, room, data);

    // Loading guard paths
    auto loadedPaths = data.getArrayField<ldtk::IntPoint>("PatrolPath");
    PatrolPathSize =  loadedPaths.size();
    int i = 0;
    for (auto& path : loadedPaths) {
        patrolPath[i] = Vector2{
            (f32) path.value().x + room->offset_x, (f32) path.value().y + room->offset_y
        };
        i++;

    }

    // Loading guards color
    const auto &enum_value = data.getField<ldtk::EnumValue>("color");
    if (enum_value == world.getEnum("color")["red"]) color = Red;
    if (enum_value == world.getEnum("color")["green"]) color = Green;
    if (enum_value == world.getEnum("color")["blue"]) color = Blue;
}
