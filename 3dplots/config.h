#pragma once
#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <sys/types.h>
#include <vector>

using std::vector;

inline constexpr int WIDTH{1000};
inline constexpr int HEIGHT{1000};
inline constexpr int FPS{60};

inline constexpr int PARTICLE_RADIUS{2};
inline constexpr int PARTICLE_MAX_SPEED{2};
inline constexpr int PARTICLE_MIN_SPEED{1};
inline constexpr int NUM_PARTICLES{4000};

inline constexpr int wavePoints{40};
inline constexpr double zoomSpeed{1.1};
inline constexpr double arrowAngle{3.14 / 7};
inline const double arrowSin{std::sin(arrowAngle)};
inline const double arrowCos{std::cos(arrowAngle)};

// Board

struct Field {
  vector<vector<vector<Vector3>>> Bfield;
  std::vector<std::vector<std::vector<double>>> magnitudes;

  // Default constructor with zeros and wavePoints
  Field()
      : Bfield(std::vector<std::vector<std::vector<Vector3>>>(
            wavePoints + 1,
            std::vector<std::vector<Vector3>>(
                wavePoints + 1,
                std::vector<Vector3>(wavePoints + 1, Vector3Zero())))),
        magnitudes(std::vector<std::vector<std::vector<double>>>(
            wavePoints + 1,
            std::vector<std::vector<double>>(
                wavePoints + 1, std::vector<double>(wavePoints + 1, 0.0)))) {}
};

extern vector<vector<vector<Vector3>>> BOARD;

Vector3 projectedVector(Vector3 &screenVector, double xRange);

Vector3 loopFunc(Vector3 &r, float theta, float radius);

double Bxfield(Vector3 &r, float radius,
               Vector3 (*loopFunc)(Vector3 &, float, float));
double Byfield(Vector3 &r, float radius,
               Vector3 (*loopFunc)(Vector3 &, float, float));
double Bzfield(Vector3 &r, float radius,
               Vector3 (*loopFunc)(Vector3 &, float, float));
Vector3 getBfield(int x, int y, int z, Vector3 &Vx, double length);
void transformArrowHead(int x, int y, int z, Vector3 &Vx, Vector3 &endArrow,
                        Model &arrowModel);
