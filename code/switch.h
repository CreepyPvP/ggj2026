
#include "entity.h"
#include "guard.h"
#include "game.h"




constexpr int max_controlled_door_size = 64;
constexpr int max_controlled_camera_size = 32;



struct Door : Entity {
    bool unlocked = false;
    bool unlockable;

    float animation_timer = -1;

    void Update(f32 delta) override;
    void Draw() override;

    void Open();

    void Configure(const ldtk::World &world, Room* room, const ldtk::Entity &data) override;

};


struct Switch : Entity {


    bool activated;

    Door* controlled_doors[max_controlled_door_size];
    GuardCamera* controlled_cameras[max_controlled_camera_size];

    i32 controlled_door_amount;
    i32 controlled_camera_amount;

    void Update(f32 delta) override;
    void Draw() override;
    void Configure(const ldtk::World &world, Room* room, const ldtk::Entity &data) override;


};


