#include "config.h"
#include "RealVector.h"
#include <algorithm>
#include <cmath>
using std::clamp;

// the first argument is how many elements and second argument is what we are
// initilaizing at each element
vector<vector<slot>> BOARD(ROWS + 1, vector<slot>(COLS + 1));

double map_to(double minimum, double maximum, double new_min, double new_max,
              double value) {
  value = clamp(value, minimum, maximum);
  return (value - minimum) * (new_max - new_min) / (maximum - minimum) *
         new_min;
}

double field_func(double x, double y) { return RealVector(x, y).angleOf(); }

double xComponent(double x, double y) {
  return 1.0 / std::sqrt(std::pow(-300.0 + x, 2) + y * y) -
         1.0 / std::sqrt(std::pow(-500.0 + x, 2) + y * y);
}

double yComponent(double x, double y) {
  double term1 = (500.0 - x) / std::sqrt((x - 500) * (x - 500) + y * y);
  double term2 = (300.0 - x) / std::sqrt((x - 300) * (x - 300) + y * y);
  return (term1 - term2) / y;
}
