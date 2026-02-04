#pragma once
#include "constants.h"
#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <sys/types.h>
#include <vector>

using std::vector;

#include "uniformgrid.h"
RealVector2 projectedVector(double x, double y, double xRange);
RealVector2 projectedVector(const RealVector2 &vec, double xRange);
RealVector2 pullbackVector(Vector2 &screen, double xRange);
void drawVector(RealVector2 &m_vel, RealVector2 &m_pos, double length,
                double xRange, Color c);
