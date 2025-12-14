#include "config.h"
#include "particle.h"
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <sys/types.h>
#include <thread>
#include <vector>

bool isRunning = true;
std::mutex gLock;
std::condition_variable gConditionVariable;

void updater(Particle &point, Vector2 &accelaration, float &dt, double xRange) {
  using clock = std::chrono::steady_clock;
  auto next = clock::now();
  while (isRunning) {
    next += std::chrono::milliseconds(30);
    {
      std::unique_lock<std::mutex> lock(gLock);
      // Do our work, because we have the lock
      //
      point.updatePar(accelaration, dt, xRange);
    }
    std::this_thread::sleep_until(next);
  }
}
//
void plotter(Particle &point, double xRange) {
  std::unique_lock<std::mutex> lock(gLock);

  point.showVel(0.35, xRange, RED, {4, 4});
  point.show();
  point.showTrace(BLUE);
}

int main() {

  InitWindow(HEIGHT, WIDTH, "Particle trajectory plot");
  // SetTargetFPS(FPS);

  double xRange{4.0}; // x will range from -4 to 4 but then changed by scrolling
  Particle pointp{{0, 0}, {4, 4}, 1, 5};
  Vector2 accelaration{0, -9.8};
  float dt{1.0f / 60.0f};
  std::thread updateThread(updater, std::ref(pointp), std::ref(accelaration),
                           std::ref(dt), xRange);

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
    DrawRectangleLines(10, 10, 80, 80, GRAY);
    plotter(pointp, xRange);

    EndDrawing();
  }

  std::cout << "Thread no " << updateThread.get_id() << " closed!" << '\n';
  updateThread.join();
  isRunning = false;
  CloseWindow();
  return 0;
}
