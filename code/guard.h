#pragma once
#include "entity.h"
#include "game.h"

constexpr float rotationSpeed = 100.0f;
constexpr float cameraRotationSpeed = 15.0f;
constexpr float speed = 2.8f;
constexpr int max_path_size = 64;

constexpr int max_camera_path_size = 16;

enum GuardColor {
  Red,
  Green,
  Blue
};

struct Guard : Entity {
    GuardColor color;
    int PatrolPathSize;
    int NextPatrolPoint;
    float ConeRotation;
    float RotationSpeedMultiplier; // between 1 and 3
    float ConeLength;
    bool RandomWalk;
    void Update(f32 delta) override;
    void Draw() override;
    void Configure(const ldtk::World &world, Room* room, const ldtk::Entity &data) override;
    Vector2 patrolPath[max_path_size];

    u32 animation_frame = 0;
    f32 animation_time = 0;
};

struct GuardCamera : Entity {
    GuardColor color;
    bool activated;
    float ConeRotation;
    int NextPatrolPoint;

    float field_of_view;

    void Update(f32 delta) override;
    void Draw() override;
    void Configure(const ldtk::World &world, Room* room, const ldtk::Entity &data) override;
    Vector2 patrolPath[max_camera_path_size];
    int PatrolPathSize;
};
