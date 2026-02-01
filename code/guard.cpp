#include "guard.h"

#include "game.h"
#include "math.h"
#include "entity.h"
#include "raymath.h"
#include "player.h"
#include "LDtkLoader/World.hpp"
#include "treasure.h"

#include <stdio.h>



void DropCash(){
    while(state.held_cash >= 30){
        int possible_treasures = 6;
        if(state.held_cash < 150)
            possible_treasures = 5;
        else if(state.held_cash < 80)
            possible_treasures = 4;
        else if(state.held_cash < 50)
            possible_treasures = 3;

        printf("Dropping money at %d", state.held_cash);


        int random_treasure = std::rand() % possible_treasures;
        TreasureType type;
        switch(random_treasure){
            case 0: type = money_small; state.held_cash -=10; break;
            case 1: type = coins_small; state.held_cash -=20; break;
            case 3: type = money_big; state.held_cash -=30; break;
            case 4: type = coins_big; state.held_cash -=50; break;
            case 2: type = diamond_small; state.held_cash -=80; break;
            case 5: type = diamond_big; state.held_cash -=150; break;
        }

        Treasure* treasure = new Treasure();
        treasure->type = type;
        float randx = (std::rand() / (float)RAND_MAX) -0.5f;
        float randy = (std::rand() / (float)RAND_MAX) -0.5f;
        treasure->position = PLAYER->position + Vector2(2*randx, 2*randy);
        AddEntity(treasure);
    }
    state.held_cash = 0;
}

void GuardCatchNoise(){
    int i = std::rand() % 2;
    if(i == 0)
        PlaySound(guardSound1);
    else
        PlaySound(guardSound2);

}

void CatchPlayer(GuardColor color, bool noise){
    if(color != PLAYER->playerColor){
        DropCash();
        GameStartLose();
        if(noise)
            GuardCatchNoise();
        else
            PlaySound(failureSound);
    }

}



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
        RotationSpeedMultiplier -= 10.0f*delta;
        if (RotationSpeedMultiplier < 1)RotationSpeedMultiplier = 1;
    }

    ConeLength = 11  - 2.5f* RotationSpeedMultiplier;

    // Check game over
    if (PLAYER && !state.game_lost)
    {
        if (Vector2DistanceSqr(position, PLAYER->position) < 0.8 * 0.8) {
            CatchPlayer(color,true);
        }

        Vector2 player_pos = PLAYER->position + Vector2{0.5, 0.775};
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
                GuardCatchNoise();
                CatchPlayer(color,true);
            }
        }
    }

    // animation
    if (animation_time > 0.1)
    {
        animation_frame = animation_frame? 0 : 1;
        animation_time -= 0.1;
    }
    animation_time += delta;
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

    int ConeLength = 10;


    //Check game over
    if (PLAYER && !state.game_lost)
    {
        Vector2 player_pos = PLAYER->position + Vector2{0.5, 0.775};
        Vector2 guard_pos = position + Vector2{0.5, 0.5};
        
        if (Vector2DistanceSqr(guard_pos, player_pos) < 0.8 * 0.8) {
            CatchPlayer(color, false);
        }

        Vector2 to = Vector2Normalize(player_pos - guard_pos);
        f32 dist = GameRaycast(guard_pos, to, ConeLength + 1);
        f32 dist_to_player = Vector2Length(player_pos - guard_pos);
        if ((dist + 0.001) >= dist_to_player && dist_to_player <= (ConeLength + 0.001))
        {
            Vector2 forward = { cos(ConeRotation / 180.0f * PI), sin(ConeRotation / 180.0f * PI) };
            Vector2 edge = { cos((ConeRotation + field_of_view/2) / 180.0f * PI), sin((ConeRotation + field_of_view/2) / 180.0f * PI) };
            if (Vector2DotProduct(forward, to) >= Vector2DotProduct(forward, edge))
            {
                CatchPlayer(color, false);
            }
        }
    }

}

void Guard::Draw() {
    Entity::Draw();
    Vector2 render_pos = {floorf(this->position.x * 32), floorf(this->position.y * 32)};

    DrawTextureRec(tileset, Rectangle{32, 416, 32, 32}, render_pos + Vector2{0.0, 15}, WHITE);
    
    Color coneColor{};

    switch (color) {
        case Red: coneColor = RED; break;
        case Green: coneColor = GREEN; break;
        case Blue: coneColor = BLUE; break;
    }
    GameDrawCone({position.x+0.5f,position.y+0.5f},ConeRotation, ConeLength, 45, coneColor);    
    
    float texture_y = 448;

    if (animation_frame)
        texture_y += 32;

    Vector2 nextPoint = patrolPath[NextPatrolPoint];
    Vector2 targetDirection = Vector2Normalize(nextPoint - position);
    Vector2 draw_dir = {targetDirection.x >= 0 ? (f32) 1 : (f32) -1, targetDirection.y >= 0 ? (f32) 1 : (f32) -1 };
    if(abs(targetDirection.x) >= abs(targetDirection.y))
        draw_dir.y = 0;
    else
        draw_dir.x = 0;

    if (draw_dir.y < 0) {
        DrawTextureRec(tileset, Rectangle{96, texture_y, 32, 32}, render_pos, WHITE);
    } else if (draw_dir.y > 0) {
        DrawTextureRec(tileset, Rectangle{32, texture_y, 32, 32}, render_pos, WHITE);
    } else if (draw_dir.x < 0) {
        DrawTextureRec(tileset, Rectangle{64, texture_y, 32, 32}, render_pos, WHITE);
    } else if (draw_dir.x > 0) {
        DrawTextureRec(tileset, Rectangle{128, texture_y, 32, 32}, render_pos, WHITE);
    } else {
        DrawTextureRec(tileset, Rectangle{32, texture_y, 32, 32}, render_pos, WHITE);
    }
}

void GuardCamera::Draw(){
    Entity::Draw();

    Vector2 render_pos = {floorf(this->position.x * 32), floorf(this->position.y * 32)};
    DrawTextureRec(tileset, Rectangle{192, 480, 32, 32}, render_pos, WHITE);

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
