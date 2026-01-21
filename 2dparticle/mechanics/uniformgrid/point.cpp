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
  SpatialGrid grid{
      {{-xRange, -xRange}, {xRange, xRange}}, {NCELLS, NCELLS}, {}};
  grid.newClient(1, {-0.1, 0.1}, {1.0f, 1.0f});

  Vector2 projectedBoundsLower{projectedVector({-xRange, -xRange}, xRange)};
  Vector2 projectedBoundsUpper{projectedVector({xRange, xRange}, xRange)};

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
    DrawRectangle(1 * WIDTH / NCELLS, 3 * HEIGHT / NCELLS, HEIGHT / NCELLS,
                  WIDTH / NCELLS, GREEN);

    // plotter(pparticles, xRange);

    EndDrawing();
  }

  // std::cout << "Thread no " << updateThread.get_id() << " closed!" << '\n';
  // updateThread.join();
  isRunning = false;
  CloseWindow();
  return 0;
}
