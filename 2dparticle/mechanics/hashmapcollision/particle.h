#pragma once
#include <mutex>
#include <raylib.h>
#include <vector>
using std::vector;

class Particle {
public:
  Vector2 m_pos{};
  Vector2 m_vel{};
  Vector2 m_indices{};
  std::vector<Vector2> trace{};

  Particle();
  Particle(Vector2 pos, Vector2 vel);

  void applyForce(Vector2 &force);
  void applyAcc(Vector2 &accelaration, float &dt);
  void updatePos(float &dt, float xRange);
  void momentumConservation(std::vector<Particle> &particleArray);
  void addToSpeeds(std::size_t i);
  static void printSpeeds();
  void binIndex();
  std::vector<int> &getBins();
  void updatePar(Vector2 &accelaration, float &dt, const float &xRange,
                 std::vector<Particle> &pparticle);
  void getTrace();
  void showTrace(Color col);
  void showVel(double length, double xRange, Color c, const Vector2 &start_vel);
  void show();

private:
  static std::vector<float> speeds;
  static std::vector<int> bins;
};

extern std::vector<Particle> particles;
void addParticle(double xRange);
void plotter(std::vector<Particle> &pparticles, double xRange);
void updater(std::vector<Particle> &pparticles, Vector2 &accelaration,
             float &dt, double xRange);
