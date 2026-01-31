#pragma once
#include "entity.h"

constexpr float rotationSpeed = 0.5f;
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
  void Configure(const ldtk::World &world, const ldtk::Entity &data) override;
  Vector2 patrolPath[32];
};