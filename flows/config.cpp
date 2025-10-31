#include "config.h"
#include "raymath.h"
#include <algorithm>
#include <boost/math/quadrature/gauss_kronrod.hpp>
#include <boost/math/quadrature/trapezoidal.hpp>

// the first argument is how many elements and second argument is what we are
// initializing at each element
vector<vector<Vector2>> BOARD(wavePoints + 1, vector<Vector2>(wavePoints + 1));
vector<vector<Vector2>> projectedBOARD(wavePoints + 1,
                                       vector<Vector2>(wavePoints + 1));

double map_to(double minimum, double maximum, double new_min, double new_max,
              double value) {
  value = std::clamp(value, minimum, maximum);
  return new_min +
         (value - minimum) * (new_max - new_min) / (maximum - minimum);
}

// These exFunc and eyFunc are for Electric fields of a line of charge
double exFunc(double &x0, double &y0, double x, double distance_y) {
  double dx = x0 - x;
  double r2 = dx * dx + (y0 - distance_y) * (y0 - distance_y); // (x-x0)^2 + y^2
  double r3_2 = r2 * r2 * std::sqrt(r2); // r^(5/2) = r^2 * sqrt(r^2)
  return dx / r3_2;
}

double eyFunc(double &x0, double &y0, double x, double distance_y) {
  double dx = x0 - x;
  double r2 = dx * dx + (y0 - distance_y) * (y0 - distance_y); // (x-x0)^2 + y^2
  double r3_2 = r2 * r2 * std::sqrt(r2); // r^(5/2) = r^2 * sqrt(r^2)
  return (y0 - distance_y) / r3_2;
}

double xCompIntegrate(double &x0, double &y0, double y_pos,
                      double (*exFunc)(double &, double &, double, double)) {
  double integral_lim{1.0};
  if (y0 == y_pos && x0 < integral_lim && x0 > -integral_lim) {
    return 0.0; // if x0,y0 on charge dont integrate just give 0
  } else {
    auto f = [&](double x) { return exFunc(x0, y0, x, y_pos); };
    return boost::math::quadrature::gauss_kronrod<double, 15>::integrate(
        f, -integral_lim, integral_lim);
  }
}

double yCompIntegrate(double &x0, double &y0, double y_pos,
                      double (*eyFunc)(double &, double &, double, double)) {
  double integral_lim{1.0};
  if (y0 == y_pos && x0 < integral_lim && x0 > -integral_lim) {
    return 0.0; // if x0,y0 on charge dont integrate just give 0
  } else {
    auto f = [&](double x) { return eyFunc(x0, y0, x, y_pos); };
    return boost::math::quadrature::gauss_kronrod<double, 15>::integrate(
        f, -integral_lim, integral_lim);
  }
}

double getMax(std::vector<vector<Vector2>> &BOARD, int ROWS, int COLS) {
  double max_length{0};
  for (int y = 0; y < ROWS; ++y) {
    for (int x = 0; x < COLS; ++x) {
      double x0{BOARD[y][x].x};
      double y0{BOARD[y][x].y};
      double x_component{xCompIntegrate(x0, y0, 1.0, exFunc)};
      double y_component{yCompIntegrate(x0, y0, 1.0, eyFunc)};
      if (max_length <
          std::abs(sqrt(pow(x_component, 2) + pow(y_component, 2))))
        max_length = std::abs(sqrt(pow(x_component, 2) + pow(y_component, 2)));
    }
  }
  return max_length;
}

Magnitudes getMaxLength(std::vector<vector<double>> &array) {
  double max{0.0};
  double min{100.0};
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

Field getEfield(double charge_position) {
  // Define an array for the magnitudes
  std::vector<std::vector<double>> magnitudes(wavePoints + 1,
                                              vector<double>(wavePoints + 1));

  vector<vector<Vector2>> Efield(wavePoints + 1,
                                 vector<Vector2>(wavePoints + 1));
  for (std::size_t y{0}; y < static_cast<std::size_t>(wavePoints); ++y) {
    for (std::size_t x{0}; x < static_cast<std::size_t>(wavePoints); ++x) {
      double x0{BOARD[y][x].x};
      double y0{BOARD[y][x].y};
      double x_component{xCompIntegrate(x0, y0, charge_position, exFunc)};
      double y_component{yCompIntegrate(x0, y0, charge_position, eyFunc)};

      // check for infty
      if (x_component > 1e+08) {
        x_component = 0;
      }
      if (y_component > 1e+08) {
        y_component = 0;
      }

      Efield[y][x] = {static_cast<float>(x_component),
                      static_cast<float>(y_component)};
      magnitudes[y][x] = Vector2Length(Efield[y][x]);
    }
  }
  return {Efield, magnitudes};
}

void drawEfield(Field &efield, std::vector<rgbValues> &colors, double length,
                double xRange) {
  Magnitudes max_length{getMaxLength(efield.magnitudes)};
  for (std::size_t y{0}; y < static_cast<std::size_t>(wavePoints); ++y) {
    for (std::size_t x{0}; x < static_cast<std::size_t>(wavePoints); ++x) {
      Color c = {
          getColorValue(efield.magnitudes[y][x], max_length.min, max_length.max,
                        colors)
              .r,
          getColorValue(efield.magnitudes[y][x], max_length.min, max_length.max,
                        colors)
              .g,
          getColorValue(efield.magnitudes[y][x], max_length.min, max_length.max,
                        colors)
              .b,
          getColorValue(efield.magnitudes[y][x], max_length.min, max_length.max,
                        colors)
              .a,
      };

      double angle{atan2(efield.Efield[y][x].y, efield.Efield[y][x].x)};
      Vector2 end = {static_cast<float>(BOARD[y][x].x + cosf(angle) * length),
                     static_cast<float>(BOARD[y][x].y + sinf(angle) * length)};
      //
      Vector2 leftWing = {projectedVector(
          end.x - cosf(angle - arrowAngle) * (length / 3),
          end.y - sinf(angle - arrowAngle) * (length / 3), xRange)};
      Vector2 rightWing = {projectedVector(
          end.x - cosf(angle + arrowAngle) * (length / 3),
          end.y - sinf(angle + arrowAngle) * (length / 3), xRange)};

      // // Map x and y values to screen coordinates
      efield.Efield[y][x] = {projectedVector(end.x, end.y, xRange)};

      DrawLineEx(efield.Efield[y][x], rightWing, 2, c);
      DrawLineEx(efield.Efield[y][x], leftWing, 2, c);
      DrawLineEx(projectedBOARD[y][x], efield.Efield[y][x], 2, c);
    }
  }
}

Vector2 projectedVector(double x, double y, double xRange) {
  Vector2 projected = {
      static_cast<float>(WIDTH / 2 + x * (WIDTH / (2 * xRange))),
      static_cast<float>(HEIGHT / 2 - y * (HEIGHT / (2 * xRange)))};
  return projected;
}

rgbValues getColorValue(double value, double minVal, double maxVal,
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
  double logVal = std::log10(value);
  double logMin = std::log10(minVal);
  double logMax = std::log10(maxVal);

  // Normalize to [0, 1]
  double normalized = (logVal - logMin) / (logMax - logMin);

  // Map to color bins
  int numBins = colors.size() - 1;
  double binIndex = normalized * numBins;
  int bin = std::min(static_cast<int>(binIndex), numBins - 1);

  if (bin < 0)
    bin = 1;
  const rgbValues &c1 = colors[bin];

  return {c1.r, c1.g, c1.b, c1.a};
}
