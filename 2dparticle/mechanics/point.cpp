#include "Random.h"
#include "config.h"
#include "particle.h"
#include <chrono>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <sys/types.h>
#include <thread>
#include <vector>

int main() {

  InitWindow(HEIGHT, WIDTH, "Particle trajectory plot");
  // SetTargetFPS(FPS);

  double xRange{4.0};

  // create particles with random initial positions and velocities
  std::vector<Particle> pparticles;

  for (int i{0}; i < NUM_PARTICLES; ++i) {
    // Vector2 initialPosition{static_cast<float>(Random::get(0, 4)),
    //                         static_cast<float>(Random::get(0, 4))};
    // Vector2 initialVelocity{static_cast<float>(Random::get(-10, 10)),
    //                         static_cast<float>(Random::get(-10, 10))};
    if (i < NUM_PARTICLES / 2) {
      Vector2 initialPosition{3, static_cast<float>(Random::get(-4, 4))};
      Vector2 initialVelocity{-3, static_cast<float>(Random::get(-4, 4))};
      pparticles.push_back(Particle{initialPosition, initialVelocity});
    }
    if (i > NUM_PARTICLES / 2) {
      Vector2 initialPosition{-3, static_cast<float>(Random::get(-4, 4))};
      Vector2 initialVelocity{3, static_cast<float>(Random::get(-4, 4))};
      pparticles.push_back(Particle{initialPosition, initialVelocity});
    }
  }
  Vector2 accelaration{0, 0};
  float dt{1.0f / 60.0f};

  std::thread updateThread(updater, std::ref(pparticles),
                           std::ref(accelaration), std::ref(dt), xRange);

  while (isRunning) {
    if (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose())
      isRunning = false;

    BeginDrawing();
    ClearBackground(BLACK);
    DrawFPS(10, 10);

    // Draw axes
    DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, GRAY);
    DrawLine(0, HEIGHT / 2, WIDTH, HEIGHT / 2, GRAY);

    DrawText("Y", WIDTH / 2 + 5, 5, 20, GRAY);
    DrawText("X", WIDTH - 20, HEIGHT / 2 + 5, 20, GRAY);
    plotter(pparticles, xRange);
    // DrawRectangleLines(10, 30, 220, 220, GRAY);
    // DrawRectangle(10, 30, 220, 220, BLACK);

    EndDrawing();
  }

  std::cout << "Thread no " << updateThread.get_id() << " closed!" << '\n';
  updateThread.join();
  isRunning = false;
  CloseWindow();
  return 0;
}
