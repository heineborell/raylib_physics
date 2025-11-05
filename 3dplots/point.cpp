#include "config.h"
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

  double xRange{
      10.0}; // x will range from -4 to 4 but then changed by scrolling
  double step{xRange * 2 / wavePoints}; // step size for plotting
  double length{step / 2};              // length of the vectors we draw

  // Initialize the camera
  Camera3D camera = {0};
  camera.position = (Vector3){30.0f, 20.0f, 30.0f}; // Camera position
  camera.target = Vector3{0.0f, 0.0f, 0.0f};        // Camera looking at point
  camera.up =
      (Vector3){0.0f, 1.0f, 0.0f}; // Camera up vector (rotation towards target)
  camera.fovy = 70.0f;             // Camera field-of-view Y
  camera.projection = CAMERA_PERSPECTIVE; // Camera projection type

  // start points set by variable step size
  for (std::size_t x{0}; x < static_cast<std::size_t>(wavePoints); ++x) {
    for (std::size_t y{0}; y < static_cast<std::size_t>(wavePoints); ++y) {
      for (std::size_t z{0}; z < static_cast<std::size_t>(wavePoints); ++z) {
        BOARD[x][y][z] = {static_cast<float>(-xRange + x * step),
                          static_cast<float>(-xRange + y * step),
                          static_cast<float>(-xRange + z * step)};
      }
    }
  }

  while (!WindowShouldClose()) {

    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode3D(camera);
    double time = GetTime();

    // // Move camera around the scene
    double cameraTime = time * 0.3;
    camera.position.x = (float)cos(cameraTime) * 40.0f;
    camera.position.z = (float)sin(cameraTime) * 40.0f;
    // //-----------------------------------------------------
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
    DrawGrid(xRange * 2, step);

    for (std::size_t x{0}; x < static_cast<std::size_t>(wavePoints); ++x) {
      for (std::size_t y{0}; y < static_cast<std::size_t>(wavePoints); ++y) {
        for (std::size_t z{0}; z < static_cast<std::size_t>(wavePoints); ++z) {
          DrawSphere((BOARD[x][y][z]), 0.2, RED);
        }
      }
    }

    //
    // for (std::size_t y{0}; y < static_cast<std::size_t>(wavePoints); ++y) {
    //   for (std::size_t x{0}; x < static_cast<std::size_t>(wavePoints); ++x) {
    //     projectedBOARD[y][x] = {
    //         projectedVector(BOARD[y][x].x, BOARD[y][x].y, xRange)};
    //   }
    // }

    EndMode3D();
    DrawFPS(10, 10);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
