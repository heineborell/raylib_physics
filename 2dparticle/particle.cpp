#include "particle.h"
#include "RealVector.h"
#include "config.h"
#include <iostream>
#include <raylib.h>
#include <raymath.h>

Particle::Particle() {};
Particle::Particle(Vector2 pos, Vector2 vel, float min_speed, float max_speed)
    : m_pos{pos}, m_vel(vel), m_min_speed(min_speed), m_max_speed(max_speed) {}

void Particle::update() {
  m_pos = Vector2Add(m_pos, m_vel);

  if (m_pos.x > 4.0f)
    m_pos.x = -4.0f;
  if (m_pos.x < -4.0f)
    m_pos.x = 4.0f;
  if (m_pos.y > 4.0f)
    m_pos.y = -4.0f;
  if (m_pos.y < -4.0f)
    m_pos.y = 4.0f;
}

void Particle::applyForce(Vector2 force, float step) {
  m_vel = Vector2Add(m_vel, force);
  m_vel = Vector2ClampValue(m_vel, 0.0, 0.03);
  // m_vel = m_vel.limit(m_min_speed, m_max_speed);
}

void Particle::show() {
  Vector2 projected{projectedVector(m_pos.x, m_pos.y, 4.0f)};
  DrawCircle(projected.x, projected.y, PARTICLE_RADIUS, GREEN);
}
