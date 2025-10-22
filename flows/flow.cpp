#include "RealVector.h"
#include "config.h"
#include "particle.h"
#include "random.h"
#include "raymath.h"
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <raylib.h>
#include <string>
#include <sys/types.h>
#include <time.h>
#include <vector>

int main() {

  InitWindow(HEIGHT, WIDTH, "Vector PLots");
  SetTargetFPS(FPS);

  float xRange{4.0f}; // x will range from -4 to 4 but then changed by scrolling
  float step{xRange * 2 / wavePoints}; // step size for plotting
  float length{step / 2};              // length of the vectors we draw
  float arrowSin{cosf(arrowAngle)};
  float arrowCos{sinf(arrowAngle)};

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
    if (xRange < 0.1f)
      xRange = 0.1f;
    if (xRange > 50.0f)
      xRange = 50.0f;

    step = xRange * 2 / wavePoints;
    length = step / 1.5;

    // start points set by variable step size
    for (std::size_t y{0}; y < static_cast<std::size_t>(wavePoints); ++y) {
      for (std::size_t x{0}; x < static_cast<std::size_t>(wavePoints); ++x) {
        slot s;
        s.start_point = {static_cast<float>(-xRange + x * step),
                         static_cast<float>(-xRange + y * step)};
        BOARD[y][x] = s;
      }
    }

    // Draw axes
    DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, GRAY);
    DrawLine(0, HEIGHT / 2, WIDTH, HEIGHT / 2, GRAY);

    DrawText("Y", WIDTH / 2 + 5, 5, 20, GRAY);
    DrawText("X", WIDTH - 20, HEIGHT / 2 + 5, 20, GRAY);

    std::vector<std::vector<float>> magnitudes(wavePoints + 1,
                                               vector<float>(wavePoints + 1));
    for (std::size_t y{0}; y < static_cast<std::size_t>(wavePoints); ++y) {
      for (std::size_t x{0}; x < static_cast<std::size_t>(wavePoints); ++x) {
        float x_component{
            xComponent(BOARD[y][x].start_point.x, BOARD[y][x].start_point.y)};
        float y_component{
            yComponent(BOARD[y][x].start_point.x, BOARD[y][x].start_point.y)};

        // check for infty
        if (x_component > 1e+04) {
          x_component = 0;
        }
        if (y_component > 1e+04) {
          y_component = 0;
        }

        BOARD[y][x].vec = {x_component, y_component};
        magnitudes[y][x] = Vector2Length(BOARD[y][x].vec);
      }
    }

    Magnitudes max_length{getMaxLength(magnitudes)};
    for (std::size_t y{0}; y < static_cast<std::size_t>(wavePoints); ++y) {
      for (std::size_t x{0}; x < static_cast<std::size_t>(wavePoints); ++x) {
        Color c = {
            getColorValue(magnitudes[y][x], max_length.min, max_length.max,
                          viridisColors)
                .r,
            getColorValue(magnitudes[y][x], max_length.min, max_length.max,
                          viridisColors)
                .g,
            getColorValue(magnitudes[y][x], max_length.min, max_length.max,
                          viridisColors)
                .b,
            getColorValue(magnitudes[y][x], max_length.min, max_length.max,
                          viridisColors)
                .a,
        };

        float angle{atan2(BOARD[y][x].vec.y, BOARD[y][x].vec.x)};
        Vector2 end = {BOARD[y][x].start_point.x + cosf(angle) * length,
                       BOARD[y][x].start_point.y + sinf(angle) * length};
        //
        Vector2 leftWing = {projectedVector(
            end.x - cosf(angle - arrowAngle) * (length / 3),
            end.y - sinf(angle - arrowAngle) * (length / 3), xRange)};
        Vector2 rightWing = {projectedVector(
            end.x - cosf(angle + arrowAngle) * (length / 3),
            end.y - sinf(angle + arrowAngle) * (length / 3), xRange)};

        // // Map x and y values to screen coordinates
        BOARD[y][x].start_point = {projectedVector(
            BOARD[y][x].start_point.x, BOARD[y][x].start_point.y, xRange)};
        BOARD[y][x].vec = {projectedVector(end.x, end.y, xRange)};

        DrawLineEx(BOARD[y][x].vec, rightWing, 2, c);
        DrawLineEx(BOARD[y][x].vec, leftWing, 2, c);
        DrawLineEx(BOARD[y][x].start_point, BOARD[y][x].vec, 2, c);
      }
    }
    // charged object drawing
    Vector2 charge_start = {projectedVector(-1, 0, xRange)};
    Vector2 charge_end = {projectedVector(1, 0, xRange)};
    DrawLineEx(charge_start, charge_end, 6, RED);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
