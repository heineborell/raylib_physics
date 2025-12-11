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
  Vector2 projected{projectedVector(m_pos, 4.0f)};
  DrawCircle(projected.x, projected.y, PARTICLE_RADIUS, GREEN);
}

void Particle::showTrace(Color col) {
  trace.push_back(m_pos);
  for (Vector2 point : trace) {
    Vector2 projected{projectedVector(point, 4.0f)};
    DrawCircle(projected.x, projected.y, 1, col);
  }
}

void Particle::showVel(double length, double xRange, Color c) {
  Vector2 x_vel{m_vel.x, 0};
  Vector2 y_vel{0, m_vel.y};
  float ratio{Vector2Length(x_vel) / Vector2Length(y_vel)};
  float sum{Vector2Length(x_vel) + Vector2Length(y_vel)};
  drawVector(x_vel, m_pos, length / (1 + 1 / ratio), xRange,
             YELLOW); // draw x resultant
  drawVector(y_vel, m_pos, length / (ratio + 1), xRange,
             YELLOW);                          // draw y resultant
  drawVector(m_vel, m_pos, length, xRange, c); // draw resultant
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
