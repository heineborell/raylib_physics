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

  if (pos.x > WIDTH)
    pos.x = 0;
  if (pos.x < 0)
    pos.x = WIDTH;
  if (pos.y > HEIGHT)
    pos.y = 0;
  if (pos.y < 0)
    pos.y = HEIGHT;
}

void Particle::applyForce(RealVector force) {
  vel = vel.add(force);
  vel = vel.limit(mns, mas);
}

void Particle::show() {
  DrawCircle(pos.x, pos.y, PARTICLE_RADIUS, {255, 0, 0, 255});
}
