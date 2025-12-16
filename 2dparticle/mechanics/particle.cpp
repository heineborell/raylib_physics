#include "particle.h"
#include "config.h"
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <thread>

bool isRunning = true;
std::mutex gLock;

Particle::Particle() {};
Particle::Particle(Vector2 pos, Vector2 vel) : m_pos{pos}, m_vel(vel) {}

void Particle::updatePos(float &dt, float xRange) {
  m_pos = Vector2Add(m_pos, Vector2Scale(m_vel, dt));

  // wall collisions
  if (m_pos.x > xRange)
    m_vel.x = -m_vel.x;
  if (m_pos.x < -xRange)
    m_vel.x = -m_vel.x;
  if (m_pos.y > xRange)
    m_vel.y = -m_vel.y;
  if (m_pos.y < -xRange)
    m_vel.y = -m_vel.y;
}

// void Particle::applyForce(Vector2 &force) {
//   m_vel = Vector2Add(m_vel, force);
//   m_vel = Vector2ClampValue(m_vel, 0.0, 0.03);
// }

void Particle::applyAcc(Vector2 &accelaration, float &dt) {
  m_vel = Vector2Add(m_vel, Vector2Scale(accelaration, dt));
}

void Particle::show() {
  Vector2 projected{projectedVector(m_pos, 4.0f)};
  DrawCircle(projected.x, projected.y, PARTICLE_RADIUS, GREEN);
}

void Particle::getTrace() {
  Vector2 projected{projectedVector(m_pos, 4.0f)};
  trace.push_back(projected);
}

void Particle::showTrace(Color col) {
  for (Vector2 point : trace) {
    DrawCircle(point.x, point.y, 1, col);
  }
}

void Particle::showVel(double length, double xRange, Color c,
                       const Vector2 &start_vel) {
  Vector2 x_vel{m_vel.x, 0};
  Vector2 y_vel{0, m_vel.y};
  float ratio{Vector2Length(x_vel) / Vector2Length(y_vel)};
  float sum{Vector2Length(x_vel) + Vector2Length(y_vel)};
  drawVector(x_vel, m_pos, abs(m_vel.x / (start_vel.x / length)), xRange,
             YELLOW); // draw x resultant, scaling with start velocity and
                      // length so that it doesn't explode on screen
  drawVector(y_vel, m_pos, abs(m_vel.y / (start_vel.y / length)), xRange,
             YELLOW);                          // draw y resultant
  drawVector(m_vel, m_pos, length, xRange, c); // draw resultant
}

void Particle::updatePar(Vector2 &accelaration, float &dt, const float &xRange,
                         std::vector<Particle> &pparticle) {
  Particle::applyAcc(accelaration, dt);
  Particle::momentumConservation(pparticle);
  Particle::updatePos(dt, xRange);
  Particle::getTrace();
}

// std::vector<Particle> particles{};
// void addParticle(double xRange) {
//   if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
//     Vector2 mouse{GetMousePosition()};
//     particles.push_back({{pullbackVector(mouse, xRange)}, {0, 0}, 1, 5});
//   }
//   if (IsKeyPressed(KEY_R))
//     particles.clear();
// }
//
//
// below is updater and plotter
//
void updater(std::vector<Particle> &pparticles, Vector2 &accelaration,
             float &dt, double xRange) {
  using clock = std::chrono::steady_clock;
  auto next = clock::now(); // take a note of current time
  while (isRunning) {
    next += std::chrono::milliseconds(30); // increment your time by delta t
    {
      std::unique_lock<std::mutex> lock(gLock);
      // Do our work, because we have the lock
      for (Particle &point : pparticles)
        point.updatePar(accelaration, dt, xRange, pparticles);
    }
    std::this_thread::sleep_until(
        next); // sleep this thread until noted time+delta t, so good thing is
               // if this your computation is longer than the delta t this
               // immediately continues
  }
}
//
void plotter(std::vector<Particle> &pparticles, double xRange) {
  std::unique_lock<std::mutex> lock(gLock);

  for (Particle &point : pparticles) {
    // point.showVel(0.35, xRange, RED, {4, 4});
    point.show();
    // point.showTrace(BLUE);
  }
}
void Particle::momentumConservation(std::vector<Particle> &pparticles) {
  for (Particle m2 : pparticles) {
    if (Vector2Equals(m_pos, m2.m_pos) && this != &m2) {
      Vector2 normal{Vector2Normalize(m_pos - m2.m_pos)};
      std::cout << "collision!" << '\n';
      Vector2 velocityDifference{m_vel - m2.m_vel};
      m_vel = m_vel - Vector2Scale(normal, Vector2DotProduct(velocityDifference,
                                                             normal));
      m2.m_vel =
          m2.m_vel +
          Vector2Scale(normal, Vector2DotProduct(velocityDifference, normal));
    }
  }
}
