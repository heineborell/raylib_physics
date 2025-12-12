#include "config.h"
#include "particle.h"
#include "raymath.h"
#include <algorithm>
#include <boost/math/quadrature/gauss_kronrod.hpp>
#include <boost/math/quadrature/trapezoidal.hpp>
#include <raylib.h>

void drawVector(Vector2 &vec_2, Vector2 &vec_1, double length, double xRange,
                Color c) {
  double angle{atan2(vec_2.y, vec_2.x)};
  Vector2 end = {static_cast<float>(vec_1.x + cosf(angle) * length),
                 static_cast<float>(vec_1.y + sinf(angle) * length)};
  //
  Vector2 leftWing = {
      projectedVector(end.x - cosf(angle - arrowAngle) * (length / 3),
                      end.y - sinf(angle - arrowAngle) * (length / 3), xRange)};
  Vector2 rightWing = {
      projectedVector(end.x - cosf(angle + arrowAngle) * (length / 3),
                      end.y - sinf(angle + arrowAngle) * (length / 3), xRange)};

  Vector2 vprojected{projectedVector(end, xRange)};
  DrawLineEx(vprojected, rightWing, 2, c);
  DrawLineEx(vprojected, leftWing, 2, c);
  DrawLineEx(projectedVector(vec_1, 4), vprojected, 2, c);
}

// Projecting to screen coordinates
Vector2 projectedVector(Vector2 vec, double xRange) {
  float width{static_cast<float>(WIDTH)};
  float height{static_cast<float>(HEIGHT)};
  Vector2 projected = {
      static_cast<float>(width / 2 + vec.x * (width / (2 * xRange))),
      static_cast<float>(height / 2 - vec.y * (height / (2 * xRange)))};
  return projected;
}

Vector2 projectedVector(double x, double y, double xRange) {
  float width{static_cast<float>(WIDTH)};
  float height{static_cast<float>(HEIGHT)};
  Vector2 projected = {
      static_cast<float>(width / 2 + x * (width / (2 * xRange))),
      static_cast<float>(height / 2 - y * (height / (2 * xRange)))};
  return projected;
}

// Projecting from screen coordinates
Vector2 pullbackVector(Vector2 &screen, double xRange) {
  Vector2 pullback = {
      static_cast<float>(((2 * screen.x - WIDTH) * xRange) / WIDTH),
      static_cast<float>(((HEIGHT - 2 * screen.y) * xRange) / HEIGHT)};
  return pullback;
}
