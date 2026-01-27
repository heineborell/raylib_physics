#include "particle.h"
#include "config.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <thread>
#include <vector>

bool isRunning = true;
std::mutex gLock;
std::vector<float> Particle::speeds(NUM_PARTICLES, 0.0f);
std::vector<int> Particle::bins(BINS, 0);

void DrawTexturedCircle(Texture2D tex, Vector2 pos, float radius) {
  Rectangle src = {0, 0, (float)tex.width, (float)tex.height};

  Rectangle dst = {pos.x, pos.y, radius * 2.0f, radius * 2.0f};

  Vector2 origin = {radius, radius};

  DrawTexturePro(tex, src, dst, origin, 0.0f, WHITE);
}

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

void Particle::applyAcc(Vector2 &accelaration, float &dt) {
  m_vel = Vector2Add(m_vel, Vector2Scale(accelaration, dt));
}

void Particle::show() {
  Vector2 projected{projectedVector(m_pos, 4.0f)};
  DrawCircle(projected.x, projected.y, PARTICLE_RADIUS, GREEN);
}

void Particle::show(Texture2D &texture) {
  Vector2 projected{projectedVector(m_pos, 4.0f)};
  DrawTexturedCircle(texture, projected, PARTICLE_RADIUS);
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
  Particle::updatePos(dt, xRange);
  Particle::momentumConservation(pparticle);
  // Particle::getTrace();
}

void Particle::addToSpeeds(std::size_t i) {
  speeds[i] = (Vector2Length(m_vel));
}

void Particle::printSpeeds() {
  for (auto i : bins)
    std::cout << i << '\n';
  std::cout << "----------------" << '\n';
}

void Particle::binIndex() {
  float minVal{*std::min_element(speeds.begin(), speeds.end())};
  float maxVal{*std::max_element(speeds.begin(), speeds.end())};
  double binWidth = (maxVal - minVal) / BINS;
  std::fill(bins.begin(), bins.end(), 0);

  for (float &speed : speeds) {
    int binNo = static_cast<int>((speed - minVal) / binWidth);
    bins[static_cast<std::size_t>(std::min(binNo, BINS - 1))] +=
        1; // here std::min is just to keep the maximum values inside a bin ;
  }
}

std::vector<int> &Particle::getBins() { return bins; }

void updater(std::vector<Particle> &pparticles, Vector2 &accelaration,
             float &dt, double xRange) {
  using clock = std::chrono::steady_clock;
  auto next = clock::now(); // take a note of current time
  while (isRunning) {
    next += std::chrono::milliseconds(16); // increment your time by delta t
    {
      std::unique_lock<std::mutex> lock(gLock);
      // Do our work, because we have the lock
      for (std::size_t i{0}; i < NUM_PARTICLES; ++i) {
        pparticles[i].updatePar(accelaration, dt, xRange, pparticles);
        pparticles[i].addToSpeeds(i);
        if (i == NUM_PARTICLES - 1) { //  the last particle update calculate
                                      //  min, max speeds and bin them
          pparticles[i].binIndex();
          // pparticles[i].printSpeeds();
        }
      }
    }
    std::this_thread::sleep_until(
        next); // sleep this thread until noted time+delta t, so good thing is
               // if this your computation is longer than the delta t this
               // immediately continues
  }
}
//
void plotter(std::vector<Particle> &pparticles, double xRange,
             std::vector<Texture2D> &texture) {
  std::unique_lock<std::mutex> lock(gLock);

  for (std::size_t i{0}; i < NUM_PARTICLES; ++i) {
    // pparticles[i].showVel(0.35, xRange, RED, {4, 4});
    if (i < NUM_PARTICLES / 2)
      pparticles[i].show(texture[0]);
    else
      pparticles[i].show(texture[1]);
    if (i == NUM_PARTICLES - 1) {
      for (int j{0}; j < BINS; ++j) {
        // DrawRectanglePro(Rectangle{10, HEIGHT - 30, HEIGHT / 4, WIDTH / 4},
        //                  {0, 0}, 270, MAROON);
        DrawRectanglePro(Rectangle{10 + j * WIDTH / (3.5f * BINS), HEIGHT - 30,
                                   WIDTH / (5 * BINS),
                                   (pparticles[i].getBins().data()[j] /
                                    static_cast<float>(NUM_PARTICLES)) *
                                       (HEIGHT / 2)},
                         {0, 0}, 180, BLUE);
      }
    }
    // pparticles[i].showTrace(BLUE);
  }
}

void plotter(std::vector<Particle> &pparticles, double xRange) {
  std::unique_lock<std::mutex> lock(gLock);

  for (std::size_t i{0}; i < NUM_PARTICLES; ++i) {
    // pparticles[i].showVel(0.35, xRange, RED, {4, 4});
    pparticles[i].show();
    if (i == NUM_PARTICLES - 1) {
      for (int j{0}; j < BINS; ++j) {
        // DrawRectanglePro(Rectangle{10, HEIGHT - 30, HEIGHT / 4, WIDTH / 4},
        //                  {0, 0}, 270, MAROON);
        DrawRectanglePro(Rectangle{10 + j * WIDTH / (3.5f * BINS), HEIGHT - 30,
                                   WIDTH / (5 * BINS),
                                   (pparticles[i].getBins().data()[j] /
                                    static_cast<float>(NUM_PARTICLES)) *
                                       (HEIGHT / 2)},
                         {0, 0}, 180, BLUE);
      }
    }
    // pparticles[i].showTrace(BLUE);
  }
}
void Particle::momentumConservation(std::vector<Particle> &particleArray) {
  for (Particle &m2 : particleArray) {
    if (this >= &m2)
      continue;
    if (Vector2Distance(m_pos, m2.m_pos) < 0.1) {
      Vector2 normal{Vector2Normalize(m_pos - m2.m_pos)};
      Vector2 velocityDifference{m_vel - m2.m_vel};
      m_vel = m_vel - Vector2Scale(normal, Vector2DotProduct(velocityDifference,
                                                             normal));
      m2.m_vel =
          m2.m_vel +
          Vector2Scale(normal, Vector2DotProduct(velocityDifference, normal));
    }
  }
}
