#include "config.h"
#include "particle.h"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <sys/types.h>
#include <vector>

int main() {

  InitWindow(HEIGHT, WIDTH, "Vector PLots");
  SetTargetFPS(FPS);

  double xRange{4.0}; // x will range from -4 to 4 but then changed by scrolling
  double step{xRange * 2 / wavePoints}; // step size for plotting
  double length{step / 2};              // length of the vectors we draw
  Particle test_particle{{0, 0}, {0, 0}, 1, 5};

  std::vector<rgbValues> viridisColors = {
      {0, 0, 255, 255},   // blue
      {0, 64, 255, 255},  // blueish
      {0, 128, 255, 255}, // sky blue
      {0, 200, 200, 255}, // cyan
      {0, 255, 0, 255},   // green
      {255, 255, 0, 255}, // yellow
      {255, 128, 0, 255}, // orange
      {255, 0, 0, 255}    // red
  };

  while (!WindowShouldClose()) {

    BeginDrawing();
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
    length = step / 2.75;

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

    Field resultant{};
    for (Vector2 point : points) {
      Vector2 screen_point{pullbackVector(point, xRange)};
      Field eField{getEfield(screen_point, 1)};
      resultant = sumFields(resultant, eField);
    }

    drawEfield(resultant, viridisColors, length, xRange);

    drawCharges();
    // Particle stuff
    Vector2 force{
        resultant
            .Efield[static_cast<int>((test_particle.m_pos.y + xRange) / step)]
                   [static_cast<int>((test_particle.m_pos.x + xRange) / step)]};
    // std::cout << test_particle.m_pos.x << "  " << test_particle.m_pos.y <<
    // '\n';
    // std::cout << force.x << "  " << force.y << '\n';
    std::cout << static_cast<int>((test_particle.m_pos.y + xRange) / step)
              << "  "
              << static_cast<int>((test_particle.m_pos.x + xRange) / step)
              << '\n';
    test_particle.applyForce(force, step);
    test_particle.update();
    test_particle.show();

    ClearBackground(BLACK);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
