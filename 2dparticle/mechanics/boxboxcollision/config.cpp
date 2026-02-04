#include "config.h"
#include <raylib.h>

// Projecting to screen coordinates
RealVector2 projectedVector(const RealVector2 &vec, double xRange) {
  double width{static_cast<double>(WIDTH)};
  double height{static_cast<double>(HEIGHT)};
  RealVector2 projected = {
      static_cast<double>(width / 2 + vec.x * (width / (2 * xRange))),
      static_cast<double>(height / 2 - vec.y * (height / (2 * xRange)))};
  return projected;
}

RealVector2 projectedVector(double x, double y, double xRange) {
  double width{static_cast<double>(WIDTH)};
  double height{static_cast<double>(HEIGHT)};
  RealVector2 projected = {
      static_cast<double>(width / 2 + x * (width / (2 * xRange))),
      static_cast<double>(height / 2 - y * (height / (2 * xRange)))};
  return projected;
}

// Projecting from screen coordinates
RealVector2 pullbackVector(RealVector2 &screen, double xRange) {
  RealVector2 pullback = {
      static_cast<double>(((2 * screen.x - WIDTH) * xRange) / WIDTH),
      static_cast<double>(((HEIGHT - 2 * screen.y) * xRange) / HEIGHT)};
  return pullback;
}
