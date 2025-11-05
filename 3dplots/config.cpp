#include "config.h"
#include "raymath.h"
#include <algorithm>
#include <boost/math/quadrature/gauss_kronrod.hpp>
#include <boost/math/quadrature/trapezoidal.hpp>
#include <raylib.h>

// the first argument is how many elements and second argument is what we are
// initializing at each element
vector<vector<vector<Vector3>>> BOARD(
    wavePoints + 1,
    vector<vector<Vector3>>(wavePoints + 1, vector<Vector3>(wavePoints + 1)));
// vector<vector<Vector2>> projectedBOARD(wavePoints + 1,
//                                        vector<Vector2>(wavePoints + 1));
// std::vector<Vector2> points{};
// std::vector<std::vector<double>> potential(wavePoints + 1,
//                                            vector<double>(wavePoints +
//                                            1));

// Projecting to screen coordinates
Vector3 projectedVector(Vector3 &screenVector, double xRange) {
  Vector3 projected = {
      static_cast<float>(WIDTH / 2 + screenVector.x * (WIDTH / (2 * xRange))),
      static_cast<float>(HEIGHT / 2 - screenVector.y * (HEIGHT / (2 * xRange))),
      static_cast<float>(WIDTH / 2 + screenVector.z * (WIDTH / (2 * xRange)))};
  return projected;
}
