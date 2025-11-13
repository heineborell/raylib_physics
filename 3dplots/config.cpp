#include "config.h"
#include "raymath.h"
#include <algorithm>
#include <boost/math/quadrature/gauss_kronrod.hpp>
#include <boost/math/quadrature/trapezoidal.hpp>
#include <memory>
#include <raylib.h>

// the first argument is how many elements and second argument is what we are
// initializing at each element
vector<vector<vector<Vector3>>> BOARD(
    wavePoints + 1,
    vector<vector<Vector3>>(wavePoints + 1, vector<Vector3>(wavePoints + 1)));

// Projecting to screen coordinates
Vector3 projectedVector(Vector3 &screenVector, double xRange) {
  Vector3 projected = {
      static_cast<float>(WIDTH / 2 + screenVector.x * (WIDTH / (2 * xRange))),
      static_cast<float>(HEIGHT / 2 - screenVector.y * (HEIGHT / (2 * xRange))),
      static_cast<float>(WIDTH / 2 + screenVector.z * (WIDTH / (2 * xRange)))};
  return projected;
}

// These are functions for integration of Biot-Savart Law
Vector3 loopFunc(Vector3 &r, float theta, float radius) {
  double rSquare{Vector3LengthSqr(r)};
  Vector3 start{radius * cos(theta), 0, radius * sin(theta)};
  Vector3 dl{Vector3Normalize(Vector3CrossProduct(start, {0, -1, 0}))};
  Vector3 dlcrossr{Vector3CrossProduct(dl, r)};
  Vector3 final{Vector3Scale(dlcrossr, 1 / rSquare)};
  return final;
}

double Bxfield(Vector3 &r, float radius,
               Vector3 (*loopFunc)(Vector3 &, float, float)) {
  double integral_lim{2 * PI};
  auto f = [&](float theta) {
    return static_cast<double>(loopFunc(r, theta, radius).x);
  };
  return boost::math::quadrature::gauss_kronrod<double, 10>::integrate(
      f, 0, integral_lim);
}

double Byfield(Vector3 &r, float radius,
               Vector3 (*loopFunc)(Vector3 &, float, float)) {
  double integral_lim{2 * PI};
  auto f = [&](float theta) {
    return static_cast<double>(loopFunc(r, theta, radius).y);
  };
  return boost::math::quadrature::gauss_kronrod<double, 10>::integrate(
      f, 0, integral_lim);
}

double Bzfield(Vector3 &r, float radius,
               Vector3 (*loopFunc)(Vector3 &, float, float)) {
  double integral_lim{2 * PI};
  auto f = [&](float theta) {
    return static_cast<double>(loopFunc(r, theta, radius).z);
  };
  return boost::math::quadrature::gauss_kronrod<double, 10>::integrate(
      f, 0, integral_lim);
}
