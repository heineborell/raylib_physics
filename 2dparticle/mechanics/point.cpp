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

  InitWindow(HEIGHT, WIDTH, "Particle trajectory plot");
  SetTargetFPS(FPS);

  double xRange{4.0}; // x will range from -4 to 4 but then changed by scrolling
  double step{xRange * 2 / wavePoints}; // step size for plotting
  double length{step / 2};              // length of the vectors we draw
  Particle pointp{{0, 0}, {4, 4}, 1, 5};
  Vector2 accelaration{0, -9.8};

  while (!WindowShouldClose()) {

    BeginDrawing();
    ClearBackground(BLACK);
    DrawFPS(10, 10);
    float dt{GetFrameTime() / 3};

    // Draw axes
    DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, GRAY);
    DrawLine(0, HEIGHT / 2, WIDTH, HEIGHT / 2, GRAY);

    DrawText("Y", WIDTH / 2 + 5, 5, 20, GRAY);
    DrawText("X", WIDTH - 20, HEIGHT / 2 + 5, 20, GRAY);

    pointp.applyAcc(accelaration, dt);
    pointp.update(dt, xRange);
    pointp.show();
    pointp.showTrace();
    // std::cout << pointp.m_pos.x << "  " << pointp.m_pos.y << '\n';
    std::cout << pointp.m_vel.x << "  " << pointp.m_vel.y << '\n';

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
