#include "Random.h"
#include "config.h"
#include "uniformgrid.h"
#include <cmath>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <sys/types.h>
#include <thread>
#include <vector>

void ToggleFullscreenWindow(int windowWidth, int windowHeight) {
  if (!IsWindowFullscreen()) {
    int monitor{GetCurrentMonitor()};
    SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
    ToggleFullscreen();
  } else {
    ToggleFullscreen();
    SetWindowSize(windowWidth, windowHeight);
  }
}

int main() {

  InitWindow(WIDTH, HEIGHT, "Particle trajectory plot");

  float xRange{4.0};
  float scaleX{WIDTH / (2 * xRange)};
  float scaleY{HEIGHT / (2 * xRange)}; // or xRange if square

  // Start Grid and Particles
  Vector2 accelaration{0, -3};
  SpatialGrid grid{{{-xRange, -xRange}, {xRange, xRange}}, {NCELLS, NCELLS}};
  float mass{1.0};
  Shape shape{Shape::ball};
  float angle{PI / 4};

  // Monkey
  grid.newClient({3, 3}, {PARTICLE_RADIUS, PARTICLE_RADIUS}, {0, 0}, mass,
                 shape);

  // Hunter
  float muzzleSpeed{13};
  grid.newClient({-3, -3}, {PARTICLE_RADIUS, PARTICLE_RADIUS},
                 {muzzleSpeed * std::cos(angle), muzzleSpeed * std::sin(angle)},
                 mass, shape);

  // Start updateThread (The physics updater)
  std::thread updateThread(&SpatialGrid::update, &grid);

  // Load textures
  std::vector<Rectangle> textureGrid{};
  Texture2D atlas = LoadTexture("../../../assets/ball_box.png");
  textureGrid.push_back(Rectangle{0 * 32, 0, 32, 32});
  textureGrid.push_back(Rectangle{1 * 32, 0, 32, 32});
  Texture2D circleTex = LoadTexture("../../../assets/face.png");

  SetTargetFPS(FPS);
  while (isRunning) {
    if (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose())
      isRunning = false;

    if (IsKeyPressed(KEY_SPACE))
      ToggleFullscreenWindow(WIDTH, HEIGHT);

    BeginDrawing();
    ClearBackground(BLACK);
    plotter(grid, atlas, xRange, scaleX);
    DrawFPS(10, 10);
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
