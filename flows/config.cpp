#include "config.h"
#include "RealVector.h"
#include "raymath.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <memory>
using std::clamp;
#include <iostream>

// the first argument is how many elements and second argument is what we are
// initializing at each element
vector<vector<slot>> BOARD(wavePoints + 1, vector<slot>(wavePoints + 1));

float map_to(float minimum, float maximum, float new_min, float new_max,
             float value) {
  value = std::clamp(value, minimum, maximum);
  return new_min +
         (value - minimum) * (new_max - new_min) / (maximum - minimum);
}

float xComponent(float x, float y) {
  // return x / (std::pow(std::pow(x, 2) + std::pow(y, 2), 3 / 2));
  return 1.0 / std::sqrt(std::pow(-1.0 + x, 2) + std::pow(y, 2)) -
         1.0 / std::sqrt(std::pow(1.0 + x, 2) + std::pow(y, 2));
}

float yComponent(float x, float y) {
  // return y / (std::pow(std::pow(x, 2) + std::pow(y, 2), 3 / 2));
  float term1 = (1.0 - x) / std::sqrt((x - 1) * (x - 1) + std::pow(y, 2));
  float term2 = (1.0 + x) / std::sqrt((x + 1) * (x + 1) + std::pow(y, 2));
  return (term1 + term2) / y;
}

float getMax(std::vector<vector<slot>> &BOARD, int ROWS, int COLS) {
  float max_length{0};
  for (int y = 0; y < ROWS; ++y) {
    for (int x = 0; x < COLS; ++x) {
      float x_component{
          xComponent(BOARD[y][x].start_point.x, BOARD[y][x].start_point.y)};
      float y_component{
          yComponent(BOARD[y][x].start_point.x, BOARD[y][x].start_point.y)};
      if (max_length <
          std::abs(sqrt(pow(x_component, 2) + pow(y_component, 2))))
        max_length = std::abs(sqrt(pow(x_component, 2) + pow(y_component, 2)));
    }
  }
  return max_length;
}

Magnitudes getMaxLength(std::vector<vector<float>> &array) {
  float max{0.0};
  float min{100.0};
  for (std::size_t y{0}; y < static_cast<std::size_t>(wavePoints); ++y) {
    for (std::size_t x{0}; x < static_cast<std::size_t>(wavePoints); ++x) {
      if (std::abs(array[y][x]) > max && !std::isinf(array[y][x]))
        max = std::abs(array[y][x]);
      if (std::abs(array[y][x]) < min & std::abs(array[y][x]) != 0)
        min = std::abs(array[y][x]);
    }
  }
  return {max, min};
}

Vector2 projectedVector(float x, float y, float xRange) {
  Vector2 projected = {WIDTH / 2 + x * (WIDTH / (2 * xRange)),
                       HEIGHT / 2 - y * (HEIGHT / (2 * xRange))};
  return projected;
}

rgbValues getColorValue(float value, float minVal, float maxVal,
                        const std::vector<rgbValues> &colors) {
  value = std::abs(value);
  // // Handle edge cases
  // if (value <= 0)
  //   return colors[0];
  // if (value <= minVal)
  //   return colors[0];
  // if (value >= maxVal)
  //   return colors.back();

  // Log transform
  float logVal = std::log10(value);
  float logMin = std::log10(minVal);
  float logMax = std::log10(maxVal);

  // Normalize to [0, 1]
  float normalized = (logVal - logMin) / (logMax - logMin);

  // Map to color bins
  int numBins = colors.size() - 1;
  float binIndex = normalized * numBins;
  int bin = std::min(static_cast<int>(binIndex), numBins - 1);

  if (bin < 0)
    bin = 1;
  const rgbValues &c1 = colors[bin];

  return {c1.r, c1.g, c1.b, c1.a};
}
