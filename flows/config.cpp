#include "config.h"
#include "RealVector.h"
#include <algorithm>
#include <cmath>
using std::clamp;

// the first argument is how many elements and second argument is what we are
// initializing at each element
vector<vector<slot>> BOARD(ROWS + 1, vector<slot>(COLS + 1));

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
  return 1.0 / std::sqrt(std::pow(-200.0 + x, 2) + std::pow(-400.0 + y, 2)) -
         1.0 / std::sqrt(std::pow(200.0 + x, 2) + std::pow(-400.0 + y, 2));
}

double yComponent(double x, double y) {
  double term1 =
      (200.0 - x) / std::sqrt((x - 200) * (x - 200) + std::pow(-400.0 + y, 2));
  double term2 =
      (200.0 + x) / std::sqrt((x + 200) * (x + 200) + std::pow(-400.0 + y, 2));
  return (term1 - term2) / 400;
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
