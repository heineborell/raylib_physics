#include "config.h"
#include "particle.h"
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
std::mutex m;
std::condition_variable cv;
bool ready = false;
bool processed = false;

int main() {

  InitWindow(HEIGHT, WIDTH, "Particle trajectory plot");
  SetTargetFPS(FPS);

  float dt{0};
  double xRange{4.0}; // x will range from -4 to 4 but then changed by scrolling
  Particle pointp{{0, 0}, {4, 4}, 1, 5};
  Vector2 accelaration{0, -9.8};
  std::thread updateThread{&Particle::updatePar, &pointp, accelaration, dt,
                           xRange};

  while (isRunning) {
    if (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose())
      isRunning = false;

    BeginDrawing();
    ClearBackground(BLACK);
    DrawFPS(10, 10);
    dt = GetFrameTime() / 3;

    // Draw axes
    DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, GRAY);
    DrawLine(0, HEIGHT / 2, WIDTH, HEIGHT / 2, GRAY);

    DrawText("Y", WIDTH / 2 + 5, 5, 20, GRAY);
    DrawText("X", WIDTH - 20, HEIGHT / 2 + 5, 20, GRAY);
    DrawRectangleLines(10, 10, 80, 80, GRAY);

    // pointp.showVel(0.35, xRange, RED, {4, 4});
    // pointp.show();
    // pointp.showTrace(BLUE);

    EndDrawing();
  }

  std::cout << "Thread no " << updateThread.get_id() << " closed!" << '\n';
  updateThread.join();
  isRunning = false;
  CloseWindow();
  return 0;
}
