#include "Random.h"
#include "config.h"
#include "particle.h"
#include "uniformgrid.h"
#include <GL/gl.h>
#include <cmath>
#include <complex>
#include <cstddef>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <sys/types.h>
#include <thread>
#include <vector>

int main() {

  InitWindow(HEIGHT, WIDTH, "Particle trajectory plot");
  // SetTargetFPS(FPS);

  float xRange{4.0};
  float scaleX{WIDTH / (2 * xRange)};
  float scaleY{HEIGHT / (2 * xRange)}; // or xRange if square
  printf("Renderer: %s\n", glGetString(GL_RENDERER));
  printf("Vendor:   %s\n", glGetString(GL_VENDOR));

  // // create particles with random initial positions and velocities
  // std::vector<Particle> pparticles;
  // float speed{4.0f};
  //
  // for (int i{0}; i < NUM_PARTICLES; ++i) {
  //   if (i < NUM_PARTICLES / 2) {
  //     int angle{Random::get(1, 20)};
  //     Vector2 initialPosition{3, static_cast<float>(Random::get(-4, 4))};
  //     Vector2 initialVelocity{-speed * std::cos(PI / (2 * angle)),
  //                             speed * std::sin(PI / (2 * angle))};
  //     pparticles.push_back(Particle{initialPosition, initialVelocity});
  //   } else {
  //     int angle{Random::get(1, 20)};
  //     Vector2 initialPosition{-3, static_cast<float>(Random::get(-4, 4))};
  //     Vector2 initialVelocity{speed * std::cos(PI / (2 * angle)),
  //                             speed * std::sin(PI / (2 * angle))};
  //     pparticles.push_back(Particle{initialPosition, initialVelocity});
  //   }
  // }
  //
  // Vector2 accelaration{0, 0};
  // float dt{1.0f / 60.0f};
  //
  // std::thread updateThread(updater, std::ref(pparticles),
  //                          std::ref(accelaration), std::ref(dt), xRange);
  //
  clientDict testClient{0, {1, 1}, {0.1, 0.1}, {{0, 1}, {1, 0}}};
  SpatialGrid grid{{{-xRange, -xRange}, {xRange, xRange}}, {NCELLS, NCELLS}};
  // grid.newClient(1, {-0.1, 0.1}, {1.0f, 1.0f});
  grid.newClient(2, {2.3f, 2.3f}, {0.03f, 0.03f});

  Vector2 projectedBoundsLower{projectedVector({-xRange, -xRange}, xRange)};
  Vector2 projectedBoundsUpper{projectedVector({xRange, xRange}, xRange)};
  Vector2 projectedTestpos{projectedVector(0.0f, 0.0f, xRange)};

  std::cout << "size of the grid " << grid.m_cells.size() << '\n';

  while (isRunning) {
    if (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose())
      isRunning = false;

    BeginDrawing();
    ClearBackground(BLACK);
    DrawFPS(10, 10);

    // Draw gridlines
    for (int y{0}; y <= HEIGHT; y = y + HEIGHT / NCELLS) {
      DrawLine(projectedBoundsLower.x, y, projectedBoundsUpper.x, y, GRAY);
    }
    for (int x{0}; x <= WIDTH; x = x + WIDTH / NCELLS) {
      DrawLine(x, projectedBoundsLower.y, x, projectedBoundsUpper.y, GRAY);
    }
    // Draw rectangles
    DrawRectangle(projectedTestpos.x - 1.0 * scaleX * 0.5f,
                  projectedTestpos.y - 1.0 * scaleY * 0.5f, 1.0 * scaleX,
                  1.0 * scaleY, RED);
    for (int i{0}; i <= grid.m_cells.size(); ++i) {
      if (grid.m_cells[i].size() > 0) {
        // std::cout << grid.m_cells[i][0] << '\n';
        // std::cout << i / NCELLS << i % NCELLS << '\n';
        DrawRectangle((i % NCELLS) * (WIDTH / NCELLS),
                      (i / NCELLS) * (HEIGHT / NCELLS), HEIGHT / NCELLS,
                      WIDTH / NCELLS, GREEN);
      }
      // std::cout << key.size() << '\n';
    }

    // plotter(pparticles, xRange);

    EndDrawing();
  }

  // std::cout << "Thread no " << updateThread.get_id() << " closed!" << '\n';
  // updateThread.join();
  isRunning = false;
  CloseWindow();
  return 0;
}
