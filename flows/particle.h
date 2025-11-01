#pragma once
#include "RealVector.h"
#include <raylib.h>
#include <vector>
using std::vector;

class Particle {
public:
  Vector2 pos;
  Vector2 vel;
  float mns, mas;

  Particle();
  Particle(Vector2 p, Vector2 v, float min_speed, float max_speed);

  void applyForce(Vector2 force);
  void update();
  void show();
};
