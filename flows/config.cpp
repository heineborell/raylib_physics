#include "config.h"
#include "RealVector.h"
#include <algorithm>
#include <cmath>
using std::clamp;

// the first argument is how many elements and second argument is what we are
// initializing at each element
vector<vector<slot>> BOARD(wavePoints + 1, vector<slot>(wavePoints + 1));

double map_to(double minimum, double maximum, double new_min, double new_max,
              double value) {
  value = std::clamp(value, minimum, maximum);
  return new_min +
         (value - minimum) * (new_max - new_min) / (maximum - minimum);
}

double field_func(const double &x, const double &y) {
  return RealVector(x, y).angleOf();
}

double xComponent(double x, double y) {
  // return x / (std::pow(std::pow(x, 2) + std::pow(y, 2), 3 / 2));
  return 1.0 / std::sqrt(std::pow(-1.0 + x, 2) + std::pow(y, 2)) -
         1.0 / std::sqrt(std::pow(1.0 + x, 2) + std::pow(y, 2));
}

double yComponent(double x, double y) {
  // return y / (std::pow(std::pow(x, 2) + std::pow(y, 2), 3 / 2));
  double term1 = (1.0 - x) / std::sqrt((x - 1) * (x - 1) + std::pow(y, 2));
  double term2 = (1.0 + x) / std::sqrt((x + 1) * (x + 1) + std::pow(y, 2));
  return (term1 + term2) / y;
}

double getMax(std::vector<vector<slot>> &BOARD, int ROWS, int COLS) {
  double max_length{0};
  for (int y = 0; y < ROWS; ++y) {
    for (int x = 0; x < COLS; ++x) {
      double x_component{
          xComponent(BOARD[y][x].start_point.x, BOARD[y][x].start_point.y)};
      double y_component{
          yComponent(BOARD[y][x].start_point.x, BOARD[y][x].start_point.y)};
      if (max_length <
          std::abs(sqrt(pow(x_component, 2) + pow(y_component, 2))))
        max_length = std::abs(sqrt(pow(x_component, 2) + pow(y_component, 2)));
    }
  }
  return max_length;
}

Vector2 projectedVector(float x, float y, float xRange) {
  Vector2 projected = {WIDTH / 2 + x * (WIDTH / (2 * xRange)),
                       HEIGHT / 2 - y * (HEIGHT / (2 * xRange))};
  return projected;
}

Vector2 Rotate(Vector2 v, float angle) {
  return {v.x * cos(angle) - v.y * sin(angle),
          v.x * sin(angle) + v.y * cos(angle)};
}
