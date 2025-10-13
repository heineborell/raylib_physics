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

  if (pos.x > GetScreenWidth())
    pos.x = 0;
  if (pos.x < 0)
    pos.x = GetScreenWidth();
  if (pos.y > GetScreenHeight())
    pos.y = 0;
  if (pos.y < 0)
    pos.y = GetScreenHeight();
}

void Particle::applyForce(RealVector force) {
  vel = vel.add(force);
  vel = vel.limit(mns, mas);
}

void Particle::show() {
  DrawCircle(pos.x, pos.y, PARTICLE_RADIUS, {255, 0, 0, 255});
}
