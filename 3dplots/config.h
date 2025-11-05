#pragma once
#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <sys/types.h>
#include <vector>

using std::vector;

inline constexpr int WIDTH{800};
inline constexpr int HEIGHT{800};
inline constexpr int FPS{60};

inline constexpr int PARTICLE_RADIUS{2};
inline constexpr int PARTICLE_MAX_SPEED{2};
inline constexpr int PARTICLE_MIN_SPEED{1};
inline constexpr int NUM_PARTICLES{4000};

inline constexpr int wavePoints{10};
inline constexpr double zoomSpeed{1.1};
inline constexpr double arrowAngle{3.14 / 7};
inline const double arrowSin{std::sin(arrowAngle)};
inline const double arrowCos{std::cos(arrowAngle)};

// Board

extern vector<vector<vector<Vector3>>> BOARD;

Vector3 projectedVector(Vector3 &screenVector, double xRange);
