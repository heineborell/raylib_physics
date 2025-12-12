#pragma once
#include "particle.h"
#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <sys/types.h>
#include <vector>

using std::vector;

inline constexpr int WIDTH{900};
inline constexpr int HEIGHT{900};
inline constexpr int FPS{60};

inline constexpr int PARTICLE_RADIUS{5};
inline constexpr int PARTICLE_MAX_SPEED{2};
inline constexpr int PARTICLE_MIN_SPEED{1};
inline constexpr int NUM_PARTICLES{4000};

inline constexpr double zoomSpeed{1.1};
inline constexpr double arrowAngle{PI / 7};
inline const double arrowSin{std::sin(arrowAngle)};
inline const double arrowCos{std::cos(arrowAngle)};

Vector2 projectedVector(double x, double y, double xRange);
Vector2 projectedVector(Vector2 vec, double xRange);
Vector2 pullbackVector(Vector2 &screen, double xRange);
Vector2 Rotate(Vector2 v, double angle);
void drawVector(Vector2 &m_vel, Vector2 &m_pos, double length, double xRange,
                Color c);
