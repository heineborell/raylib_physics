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
inline constexpr int NCELLS{120};
inline float dt{1.0f / 30.0f};

inline constexpr float PARTICLE_RADIUS{0.6};
inline constexpr int PARTICLE_MAX_SPEED{2};
inline constexpr int PARTICLE_MIN_SPEED{1};
inline constexpr int NUM_PARTICLES{100};
inline constexpr int BINS{50};

inline constexpr double zoomSpeed{1.1};
inline constexpr double arrowAngle{PI / 7};
inline const double arrowSin{std::sin(arrowAngle)};
inline const double arrowCos{std::cos(arrowAngle)};
extern bool isRunning;

Vector2 projectedVector(double x, double y, float xRange);
Vector2 projectedVector(const Vector2 &vec, float xRange);
Vector2 pullbackVector(Vector2 &screen, double xRange);
void drawVector(Vector2 &m_vel, Vector2 &m_pos, double length, double xRange,
                Color c);
