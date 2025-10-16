#include "particle.h"
#include "RealVector.h"
#include "config.h"
#include <iostream>
#include <raylib.h>

Particle::Particle() {};
Particle::Particle(RealVector p, RealVector v, float min_speed,
                   float max_speed) {
  pos = p;
  vel = v;
  mns = min_speed;
  mas = max_speed;
}

void Particle::update() {
  pos = pos.add(vel);

  if (pos.x > 4.0f)
    pos.x = 0;
  if (pos.x < 0)
    pos.x = 4.0f;
  if (pos.y > 4.0f)
    pos.y = 0;
  if (pos.y < 0)
    pos.y = 4.0f;
}

void Particle::applyForce(RealVector force) {
  vel = vel.add(force);
  vel = vel.limit(mns, mas);
}

void Particle::show() {
  Vector2 projected{projectedVector(pos.x, pos.y, 4.0f)};
  DrawCircle(projected.x, projected.y, PARTICLE_RADIUS, {255, 0, 0, 255});
}
