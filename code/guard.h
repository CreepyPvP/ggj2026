#pragma once
#include "entity.h"
#include "game.h"

constexpr float rotationSpeed = 100.0f;
constexpr float speed = 3.0f;
constexpr int max_path_size = 32;

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
  bool RandomWalk;
  void Update(f32 delta) override;
  void Draw() override;
  void Configure(const ldtk::World &world, Room* room, const ldtk::Entity &data) override;
  Vector2 patrolPath[32];
};