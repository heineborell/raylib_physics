#pragma once
#include "RealVector.h"
#include <raylib.h>
#include <vector>
using std::vector;

struct Particle {
  Vector2 m_pos{};
  Vector2 m_vel{};
  float m_min_speed{};
  float m_max_speed{};

  Particle();
  Particle(Vector2 pos, Vector2 vel, float min_speed, float max_speed);

  void applyForce(Vector2 force, float step);
  void update();
  void show();
};

extern std::vector<Particle> particles;
void addParticle(double xRange);
