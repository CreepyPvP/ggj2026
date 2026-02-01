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
        //RotationSpeedMultiplier = 1.0f;
    }

    movementDirection = Vector2Normalize(movementDirection);
    Vector2 facingDirection = Vector2(cos(ConeRotation/ 180.0f * PI), sin(ConeRotation/ 180.0f * PI));
    float determinante = movementDirection.x * facingDirection.y - movementDirection.y * facingDirection.x;

    float dotProduct = movementDirection.x * facingDirection.x + movementDirection.y * facingDirection.y;
    if (dotProduct < 0.999f) { //Vector2DistanceSqr(facingDirection, {0,0}) = 1 unless floating point precision...
        ConeRotation += (determinante >= 0 ? -1 : 1 ) * RotationSpeedMultiplier * rotationSpeed* delta;
    }

    if ((dotProduct+0.25) * 3 < RotationSpeedMultiplier) {
        RotationSpeedMultiplier += 10*delta;
        if (RotationSpeedMultiplier > 3)RotationSpeedMultiplier = 3;
    }
    else {
        RotationSpeedMultiplier -= 1.5f*delta;
        if (RotationSpeedMultiplier < 1)RotationSpeedMultiplier = 1;
    }

    ConeLength = 12  - 3* RotationSpeedMultiplier;

    // Check game over
    if (PLAYER)
    {
        Vector2 player_pos = PLAYER->position + Vector2{0.5, 0.5};
        Vector2 guard_pos = position + Vector2{0.5, 0.5};
        Vector2 to = Vector2Normalize(player_pos - guard_pos);
        f32 dist = GameRaycast(guard_pos, to, ConeLength + 1);
        f32 dist_to_player = Vector2Length(player_pos - guard_pos);
        if ((dist + 0.001) >= dist_to_player && dist_to_player <= (ConeLength + 0.001))
        {
            Vector2 forward = { cos(ConeRotation / 180.0f * PI), sin(ConeRotation / 180.0f * PI) };
            Vector2 edge = { cos((ConeRotation + 22.5f) / 180.0f * PI), sin((ConeRotation + 22.5f) / 180.0f * PI) };
            if (Vector2DotProduct(forward, to) >= Vector2DotProduct(forward, edge))
            {
                GameStartLose();
            }
        }
    }
}

void GuardCamera::Update(f32 delta){
    if(!activated) return;
    if(PatrolPathSize > 1){

        Vector2 nextPoint = patrolPath[NextPatrolPoint];
        Vector2 targetDirection = Vector2Normalize(nextPoint - position);
    
        Vector2 facingDirection = Vector2(cos(ConeRotation/ 180.0f * PI), sin(ConeRotation/ 180.0f * PI));
        float determinante = targetDirection.x * facingDirection.y - targetDirection.y * facingDirection.x;

        float dotProduct = targetDirection.x * facingDirection.x + targetDirection.y * facingDirection.y;
        if (dotProduct < 0.999f * Vector2DistanceSqr(facingDirection, {0,0})) {
            ConeRotation += (determinante >= 0 ? -1 : 1 ) * cameraRotationSpeed* delta;
        }else{
            NextPatrolPoint = (NextPatrolPoint + 1) % PatrolPathSize;
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
    GameDrawCone({position.x+0.5f,position.y+0.5f},ConeRotation, ConeLength, 45, coneColor);
}

void GuardCamera::Draw(){
    Entity::Draw();

    Vector2 render_pos = {floorf(this->position.x * 32), floorf(this->position.y * 32)};
    DrawTextureRec(tileset, Rectangle{192, 480, 32, 32}, render_pos, {255,255,255,255});

    if(!activated) return;
    Color coneColor{};

    switch (color) {
        case Red: coneColor = RED; break;
        case Green: coneColor = GREEN; break;
        case Blue: coneColor = BLUE; break;
    }
    GameDrawCone({position.x+0.5f,position.y+0.5f},ConeRotation, 10, field_of_view, coneColor);


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

void GuardCamera::Configure(const ldtk::World &world, Room* room, const ldtk::Entity &data) {
    Entity::Configure(world, room, data);

    field_of_view = data.getField<ldtk::FieldType::Float>("field_of_view").value();

    activated = data.getField<ldtk::FieldType::Bool>("active").value();

    // Loading guard paths
    auto loadedPaths = data.getArrayField<ldtk::IntPoint>("directions");
    PatrolPathSize =  loadedPaths.size();
    int i = 0;
    for (auto& path : loadedPaths) {
        patrolPath[i] = Vector2{
            (f32) path.value().x + room->offset_x, (f32) path.value().y + room->offset_y
        };
        i++;
    }
    if(i>0){
        Vector2 startDirection = Vector2Normalize(patrolPath[0] - position);
        ConeRotation = -Vector2Angle(startDirection, {1,0}) * 180 / PI;

    }

    // Loading guards color
    const auto &enum_value = data.getField<ldtk::EnumValue>("color");
    if (enum_value == world.getEnum("color")["red"]) color = Red;
    if (enum_value == world.getEnum("color")["green"]) color = Green;
    if (enum_value == world.getEnum("color")["blue"]) color = Blue;
}
