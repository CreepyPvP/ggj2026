#include "guard.h"

#include "game.h"
#include "entity.h"
#include "raymath.h"
#include "player.h"
#include "LDtkLoader/World.hpp"

#include <stdio.h>

void Guard::Update(f32 delta) {
    Entity::Update(delta);


    Vector2 nextPoint = patrolPath[NextPatrolPoint];
    Vector2 movementDirection = Vector2Scale(Vector2Normalize((nextPoint - position)), speed*delta);
    position = movementDirection + position;

    if (Vector2DistanceSqr(position + movementDirection, nextPoint ) > Vector2DistanceSqr(position,nextPoint)) {
        NextPatrolPoint = (NextPatrolPoint + 1) % PatrolPathSize;
    }

    movementDirection = Vector2Normalize(movementDirection);
    Vector2 facingDirection = Vector2(cos(ConeRotation/ 180.0f * PI), sin(ConeRotation/ 180.0f * PI));
    float determinante = movementDirection.x * facingDirection.y - movementDirection.y * facingDirection.x;

    float dotProduct = movementDirection.x * facingDirection.x + movementDirection.y * facingDirection.y;
    if (dotProduct < 0.999f * Vector2DistanceSqr(facingDirection, {0,0})) {
        ConeRotation += (determinante >= 0.1f ? -1 : 1 ) * rotationSpeed* delta;
    }

    // Check game over
    if (PLAYER)
    {
        Vector2 player_pos = PLAYER->position + Vector2{0.5, 0.5};
        Vector2 to = Vector2Normalize(player_pos - position);
        f32 dist = GameRaycast(position, to, 10);
        if (dist - 0.001 >= Vector2Length(player_pos - position) && dist < 10)
        {
            Vector2 forward = { cos(ConeRotation / 180 * PI), sin (ConeRotation / 180 * PI) };
            Vector2 edge = { cos((ConeRotation + 45) / 180 * PI), sin((ConeRotation + 45) / 180 * PI) };
            if (Vector2DotProduct(forward, to) >= Vector2DotProduct(forward, edge))
            {
                GameStartLose();
            }
        }
    }
}

void Guard::Draw() {
    Entity::Draw();

    Vector2 render_pos = {floorf(this->position.x * 32), floorf(this->position.y * 32)};
    DrawTextureRec(tileset, Rectangle{32, 448, 32, 32}, render_pos, {255,255,255,255});

    Color coneColor{};

    switch (color) {
        case Red: coneColor = RED; break;
        case Green: coneColor = GREEN; break;
        case Blue: coneColor = BLUE; break;
    }
    GameDrawCone({position.x+0.5f,position.y+0.5f},ConeRotation, 10, 45, coneColor);
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
