#include "particle.h"
#include "config.h"
#include <iostream>
#include <raylib.h>
#include <raymath.h>

Particle::Particle() {};
Particle::Particle(Vector2 pos, Vector2 vel, float min_speed, float max_speed)
    : m_pos{pos}, m_vel(vel), m_min_speed(min_speed), m_max_speed(max_speed) {}

void Particle::update(float dt, float xRange) {
  m_pos = Vector2Add(m_pos, Vector2Scale(m_vel, dt));

  // wrap around
  if (m_pos.x > xRange)
    m_pos.x = -xRange;
  if (m_pos.x < -xRange)
    m_pos.x = xRange;
  if (m_pos.y > xRange)
    m_pos.y = -xRange;
  if (m_pos.y < -xRange)
    m_pos.y = xRange;
}

void Particle::applyForce(Vector2 force, float step) {
  m_vel = Vector2Add(m_vel, force);
  m_vel = Vector2ClampValue(m_vel, 0.0, 0.03);
  // m_vel = m_vel.limit(m_min_speed, m_max_speed);
}

void Particle::applyAcc(Vector2 accelaration, float dt) {
  m_vel = Vector2Add(m_vel, Vector2Scale(accelaration, dt));
}

void Particle::show() {
  Vector2 projected{projectedVector(m_pos.x, m_pos.y, 4.0f)};
  DrawCircle(projected.x, projected.y, PARTICLE_RADIUS, GREEN);
}

void Particle::showTrace() {
  trace.push_back(m_pos);
  for (Vector2 point : trace) {
    Vector2 projected{projectedVector(point.x, point.y, 4.0f)};
    DrawCircle(projected.x, projected.y, 1, RED);
  }
}

std::vector<Particle> particles{};
void addParticle(double xRange) {
  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    Vector2 mouse{GetMousePosition()};
    particles.push_back({{pullbackVector(mouse, xRange)}, {0, 0}, 1, 5});
  }
  if (IsKeyPressed(KEY_R))
    particles.clear();
}
