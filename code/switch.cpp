#include "switch.h"

#include "game.h"
#include "game_math.h"
#include "player.h"
#include "raymath.h"
#include "LDtkLoader/World.hpp"
#include "rlgl.h"




void Switch::Update(f32 delta) {
    Entity::Update(delta);

    if (activated) return;
    
    if (Vector2DistanceSqr(position, PLAYER->position) < 0.8 * 0.8) {
        activated = true;
        for(i32 i = 0 ; i < controlled_door_amount; i++){
            controlled_doors[i]->Open();
        }
        for(i32 i = 0 ; i < controlled_camera_amount; i++){
            if(controlled_cameras[i])
                controlled_cameras[i]->activated = !controlled_cameras[i]->activated;
        }
    }
}

void Door::Update(f32 delta){
    if(animation_timer >= 0){
        animation_timer += 2*delta;
        if(animation_timer >= 2)
            animation_timer = -1;
    }
}

void Switch::Draw() {
    Entity::Draw();
    if (activated) return;
    Vector2 render_pos = {floorf(this->position.x * 32), floorf(this->position.y * 32)};
    DrawTextureRec(tileset, Rectangle{32*10, 448, 32, 32}, render_pos, {255,255,255,255});

}

void Door::Draw(){
    Entity::Draw();
    
    Vector2 render_pos = {floorf(this->position.x * 32), floorf(this->position.y * 32)};
    if(!unlocked){
        DrawTextureRec(tileset, Rectangle{576, 96, 32, 32}, render_pos, {255,255,255,255});
    }else{
        if(animation_timer >= 0){
            if(animation_timer <= 1){
                float t = Range(animation_timer,0, 1);
                t = EaseOutExpo(t);
                rlPushMatrix();
                rlTranslatef(render_pos.x+16, render_pos.y+16, 0);
                rlScalef(1+t, 1+t, 1);
                rlTranslatef(-render_pos.x-16, -render_pos.y-16, 0);
                DrawTextureRec(tileset, Rectangle{576, 96, 32, 32}, render_pos, {255,255,255,255});
                rlPopMatrix();
            }else{
                float t = Range(animation_timer-1,0, 1);
                t = EaseOutExpo(1-t);
                rlPushMatrix();
                rlTranslatef(render_pos.x+16, render_pos.y+16, 0);
                rlScalef(2*t, 2*t, 1);
                rlTranslatef(-render_pos.x-16, -render_pos.y-16, 0);
                DrawTextureRec(tileset, Rectangle{576, 96, 32, 32}, render_pos, {255,255,255,255});
                rlPopMatrix();
            }
        }
    }



}

void Door::Open() {
    unlocked = true;
    unlockable = false;
    SetTile(position.x, position.y, 0);

    animation_timer = 0;


}


void Switch::Configure(const ldtk::World &world, Room* room, const ldtk::Entity &data) {
    Entity::Configure(world, room, data);

    activated = false;
    
    auto loadedDoors = data.getArrayField<ldtk::EntityRef>("Controlled_doors");
    controlled_door_amount = loadedDoors.size();
    int i = 0;
    for (ldtk::Field<ldtk::EntityRef> door_ref : loadedDoors) {
        controlled_doors[i] = (Door*)getEntity(door_ref.value()->iid);
        i++;
    }

    auto loadedCameras = data.getArrayField<ldtk::EntityRef>("Controlled_cameras");
    controlled_camera_amount = loadedCameras.size();
    for (ldtk::Field<ldtk::EntityRef> camera_ref : loadedCameras) {
        controlled_cameras[i] = (GuardCamera*)getEntity(camera_ref.value()->iid);
        i++;
    }

}


void Door::Configure(const ldtk::World &world, Room* room, const ldtk::Entity &data) {
    unlockable = data.getField<ldtk::FieldType::Bool>("unlockable").value();



    

}


