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
  Vector2 accelaration{0, 0};
  SpatialGrid grid{{{-xRange, -xRange}, {xRange, xRange}}, {NCELLS, NCELLS}};
  for (int i{0}; i < NUM_PARTICLES; ++i) {

    int angle{Random::get(1, 20)};
    float speed{static_cast<float>(Random::get(-3, 3))};
    Vector2 initialPosition{-3.9f + (Random::get(0, 7800) / 1000.0f),
                            -3.9f + (Random::get(0, 7800) / 1000.0f)};
    Vector2 initialVelocity{-speed * std::cos(PI / (2 * angle)),
                            speed * std::sin(PI / (2 * angle))};

    Vector2 dimensions{PARTICLE_RADIUS, PARTICLE_RADIUS};
    float mass{1.0};
    Shape shape{Shape::ball};
    grid.newClient(initialPosition, dimensions, initialVelocity, mass, shape);
  }
  // Vector2 initialPositionSmall{4, -4};
  // Vector2 initialVelocitySmall{3, 3};
  // Vector2 dimensionsSmall{0.7, 0.7};
  // double massSmall{20};
  //
  // Shape shape{Shape::ball};
  // grid.newClient(initialPositionSmall, dimensionsSmall, initialVelocitySmall,
  //                massSmall, shape);

  // Start updateThread (The physics updater)
  std::thread updateThread(&SpatialGrid::update, &grid);

  // Load textures
  std::vector<Rectangle> textureGrid{};
  Texture2D atlas =
      LoadTexture("../../../assets/colored_ball_shadow-Sheet.png");
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
