#include "config.h"
#include "particle.h"
#include "raymath.h"
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <raylib.h>
#include <sys/types.h>
#include <vector>

int main() {

  InitWindow(HEIGHT, WIDTH, "Vector PLots");
  SetTargetFPS(FPS);

  double xRange{4.0}; // x will range from -4 to 4 but then changed by scrolling
  double step{xRange * 2 / wavePoints}; // step size for plotting
  double length{step / 2};              // length of the vectors we draw

  std::vector<rgbValues> viridisColors = {
      {53, 42, 135, 255},   // dark blue
      {19, 123, 190, 255},  // blue
      {120, 190, 125, 255}, // green
      {250, 250, 110, 255}  // yellow
  };

  while (!WindowShouldClose()) {

    BeginDrawing();
    ClearBackground(BLACK);
    DrawFPS(10, 10);

    if (IsKeyDown(KEY_UP))
      xRange /= zoomSpeed; // zoom in
    if (IsKeyDown(KEY_DOWN))
      xRange *= zoomSpeed; // zoom in
    if (xRange < 0.1)
      xRange = 0.1f;
    if (xRange > 50.0)
      xRange = 50.0;

    step = xRange * 2 / wavePoints;
    length = step / 1.5;

    // start points set by variable step size
    for (std::size_t y{0}; y < static_cast<std::size_t>(wavePoints); ++y) {
      for (std::size_t x{0}; x < static_cast<std::size_t>(wavePoints); ++x) {
        BOARD[y][x] = {static_cast<float>(-xRange + x * step),
                       static_cast<float>(-xRange + y * step)};
      }
    }

    for (std::size_t y{0}; y < static_cast<std::size_t>(wavePoints); ++y) {
      for (std::size_t x{0}; x < static_cast<std::size_t>(wavePoints); ++x) {
        projectedBOARD[y][x] = {
            projectedVector(BOARD[y][x].x, BOARD[y][x].y, xRange)};
      }
    }
    // Draw axes
    DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, GRAY);
    DrawLine(0, HEIGHT / 2, WIDTH, HEIGHT / 2, GRAY);

    DrawText("Y", WIDTH / 2 + 5, 5, 20, GRAY);
    DrawText("X", WIDTH - 20, HEIGHT / 2 + 5, 20, GRAY);

    Field efield_1{getEfield(1.0)};
    drawEfield(efield_1, viridisColors, length, xRange);

    Field efield_2{getEfield(-1.0)};
    drawEfield(efield_2, viridisColors, length, xRange);

    // charged object drawing
    Vector2 charge_start = {projectedVector(-1, 1, xRange)};
    Vector2 charge_end = {projectedVector(1, 1, xRange)};
    Vector2 charge_2_start = {projectedVector(-1, -1, xRange)};
    Vector2 charge_2_end = {projectedVector(1, -1, xRange)};
    DrawLineEx(charge_start, charge_end, 3, RED);
    DrawLineEx(charge_2_start, charge_2_end, 3, RED);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
