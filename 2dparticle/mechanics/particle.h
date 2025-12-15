#pragma once
#include <mutex>
#include <raylib.h>
#include <vector>
using std::vector;

struct Particle {
  Vector2 m_pos{};
  Vector2 m_vel{};
  std::vector<Vector2> trace{};

  Particle();
  Particle(Vector2 pos, Vector2 vel);

  void applyForce(Vector2 &force);
  void applyAcc(Vector2 &accelaration, float &dt);
  void updatePos(float &dt, float xRange);
  void updatePar(Vector2 &accelaration, float &dt, const float &xRange);
  void getTrace();
  void showTrace(Color col);
  void showVel(double length, double xRange, Color c, const Vector2 &start_vel);
  void show();
};

extern std::vector<Particle> particles;
void addParticle(double xRange);
