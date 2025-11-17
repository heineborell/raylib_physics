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

Vector3 getBfield(int x, int y, int z, Vector3 &Vx, double length) {

  // Compute unit vector of the B field B=vXr
  Vector3 unitVec = Vector3Normalize(Vector3CrossProduct(Vx, BOARD[x][y][z]));
  Vector3 unitR = Vector3Normalize(BOARD[x][y][z]);

  Vector3 endArrow = Vector3Add(BOARD[x][y][z], Vector3Scale(unitVec, length));
  return endArrow;
}

void transformArrowHead(int x, int y, int z, Vector3 &Vx, Vector3 &endArrow,
                        Model &arrowModel) {

  // This is for the arrow cylinder. First compute rotation to align +Y
  // with B field at each point. Then compute quaternion for rotation
  // and finally transform
  Vector3 unitVec = Vector3Normalize(Vector3CrossProduct(Vx, BOARD[x][y][z]));
  Vector3 up = {0, 1, 0};
  Vector3 axis = Vector3CrossProduct(up, unitVec);
  float axisLength = Vector3Length(axis);
  float arrowAngle = acosf(Vector3DotProduct(up, unitVec));

  // Handle edge cases (parallel or anti-parallel vectors)
  if (axisLength < 0.0001f) {
    // when unitvec is nearly parallel or opposite to +Y
    if (unitVec.y < 0.0f) { // opposite
      axis = (Vector3){1, 0, 0};
      arrowAngle = PI;
    } else { // same direction
      axis = (Vector3){1, 0, 0};
      arrowAngle = 0;
    }
  }
  Quaternion q = QuaternionFromAxisAngle(Vector3Normalize(axis), arrowAngle);
  arrowModel.transform = QuaternionToMatrix(q);
}
