#include "Random.h"
#include "config.h"
#include "uniformgrid.h"
#include <cmath>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <sys/types.h>
#include <thread>
#include <vector>

int main() {

  InitWindow(HEIGHT, WIDTH, "Particle trajectory plot");

  float xRange{20.0};
  float scaleX{WIDTH / (2 * xRange)};
  float scaleY{HEIGHT / (2 * xRange)}; // or xRange if square

  // Start Grid and Particles
  Vector2 accelaration{0, 0};
  SpatialGrid grid{{{-xRange, -xRange}, {xRange, xRange}}, {NCELLS, NCELLS}};

  Vector2 initialPositionSmall{-18, 2};
  Vector2 initialVelocitySmall{0, 0};
  Vector2 initialPositionBig{-15, 2};
  Vector2 initialVelocityBig{-2, 0};
  Vector2 dimensionsSmall{1, 1};
  Vector2 dimensionsBig{4, 4};
  float massSmall{1};
  float massBig{1E6};

  Shape shape{Shape::ball};
  grid.newClient(initialPositionSmall, dimensionsSmall, initialVelocitySmall,
                 massSmall, shape);
  grid.newClient(initialPositionBig, dimensionsBig, initialVelocityBig, massBig,
                 shape);

  // Start updateThread (The physics updater)
  std::thread updateThread(&SpatialGrid::update, &grid);

  // Load textures
  std::vector<Rectangle> textureGrid{};
  Texture2D atlas = LoadTexture("../../../assets/ball_box.png");
  textureGrid.push_back(Rectangle{0 * 32, 0, 32, 32});
  textureGrid.push_back(Rectangle{1 * 32, 0, 32, 32});
  Texture2D circleTex = LoadTexture("../../../assets/face.png");
  // char counterText = static_cast<char>(counter);

  SetTargetFPS(FPS);
  while (isRunning) {
    if (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose())
      isRunning = false;

    if (IsKeyPressed(KEY_SPACE))
      ToggleFullscreen();

    BeginDrawing();
    ClearBackground(BLACK);
    plotter(grid, atlas, xRange, scaleX);
    std::cout << counter << '\n';
    DrawFPS(10, 10);

    // Draw axes
    DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, GRAY);
    DrawLine(0, HEIGHT / 2, WIDTH, HEIGHT / 2, GRAY);

    DrawText("Y", WIDTH / 2 + 5, 5, 20, GRAY);
    DrawText("X", WIDTH - 20, HEIGHT / 2 + 5, 20, GRAY);

    DrawText(TextFormat("Number of Collisions:%i", static_cast<int>(counter)),
             10, 30, 60,
             YELLOW); // draw equation on text

    dt = GetFrameTime();
    EndDrawing();
  }

  std::cout << "Thread no " << updateThread.get_id() << " closed!" << '\n';
  updateThread.join();
  UnloadTexture(circleTex);
  UnloadTexture(atlas);
  isRunning = false;
  CloseWindow();
  return 0;
}
