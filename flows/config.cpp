#include "config.h"
#include "RealVector.h"
#include <algorithm>
#include <cmath>
using std::clamp;

// the first argument is how man elements and second argument is what we are
// initilaizing at each element
vector<vector<slot>> BOARD(ROWS + 1, vector<slot>(COLS + 1));

double map_to(double minimum, double maximum, double new_min, double new_max,
              double value) {
  value = clamp(value, minimum, maximum);
  return (value - minimum) * (new_max - new_min) / (maximum - minimum) *
         new_min;
}

double field_func(double x, double y) {
  return RealVector(cos(y), sin(x)).angleOf();
}
