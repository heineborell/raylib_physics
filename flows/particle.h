#pragma once
#include "RealVector.h"
#include <raylib.h>
#include <vector>
using std::vector;

class Particle {
public:
  RealVector pos;
  RealVector vel;
  float mns, mas;

  Particle();
  Particle(RealVector p, RealVector v, float min_speed, float max_speed);

  void applyForce(RealVector force);
  void update();
  void show();
};
