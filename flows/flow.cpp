#include "RealVector.h"
#include "config.h"
#include "particle.h"
#include "random.h"
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <raylib.h>
#include <string>
#include <sys/types.h>
#include <time.h>

int main() {

  InitWindow(HEIGHT, WIDTH, "Vector PLots");
  SetTargetFPS(FPS);

  // get the max length for coloring purposes
  // double max_length{getMax(BOARD, COLS, ROWS)};

  float xRange{4.0f}; // x will range from -4 to 4 but then changed by scrolling
  float step{xRange * 2 / wavePoints}; // step size for plotting
  float length{step / 2};              // length of the vectors we draw

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

    for (std::size_t y{0}; y < static_cast<std::size_t>(wavePoints); ++y) {
      for (std::size_t x{0}; x < static_cast<std::size_t>(wavePoints); ++x) {
        double x_component{
            xComponent(BOARD[y][x].start_point.x, BOARD[y][x].start_point.y)};
        double y_component{
            yComponent(BOARD[y][x].start_point.x, BOARD[y][x].start_point.y)};
        double angle = field_func(x_component, y_component);
        BOARD[y][x].vec = RealVector(cos(angle) * length, sin(angle) * length);
        Vector2 end = {BOARD[y][x].start_point.x + BOARD[y][x].vec.x,
                       BOARD[y][x].start_point.y + BOARD[y][x].vec.y};

        // // Map x and y values to screen coordinates
        Vector2 projectedStart{projectedVector(
            BOARD[y][x].start_point.x, BOARD[y][x].start_point.y, xRange)};
        Vector2 projectedEnd{projectedVector(end.x, end.y, xRange)};

        DrawLineEx(projectedStart, projectedEnd, 1, (Color){0, 255, 0, 255});
      }
    }

    // charged object drawing
    Vector2 charge_start = {projectedVector(-1, 0, xRange)};
    Vector2 charge_end = {projectedVector(1, 0, xRange)};
    DrawLineEx(charge_start, charge_end, 4, RED);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
