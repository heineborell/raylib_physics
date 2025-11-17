#include "config.h"
#include "rlgl.h"
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
      20.0}; // x will range from -4 to 4 but then changed by scrolling
  double step{xRange * 2 / wavePoints}; // step size for plotting
  double length{step / 2};              // length of the vectors we draw

  // Define the camera to look into our 3d world
  Camera3D camera = {0};
  camera.position = (Vector3){10.0f, 10.0f, 10.0f}; // Camera position
  camera.target = (Vector3){0.0f, 0.0f, 0.0f};      // Camera looking at point
  camera.up =
      (Vector3){0.0f, 1.0f, 0.0f}; // Camera up vector (rotation towards target)
  camera.fovy = 45.0f;             // Camera field-of-view Y
  camera.projection = CAMERA_PERSPECTIVE;
  DisableCursor();

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

  // Define cylinder Mesh for the velocity vector. Not quite needed but whatever
  Mesh cylinderMesh = GenMeshCylinder(0.1f, static_cast<float>(2 * xRange), 20);
  Model cylinderModel = LoadModelFromMesh(cylinderMesh);
  cylinderModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = MAROON;

  // Define cone mesh for the arrow Heads
  Mesh arrow = GenMeshCone(0.03f, 0.1f, 5);
  Model arrowModel = LoadModelFromMesh(arrow);
  arrowModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = GREEN;

  step = xRange * 2 / wavePoints;
  length = step / 2.75;
  // DrawGrid(40, 1);
  // Draw charge and its velocity
  // DrawSphere({0, 0, 0}, 0.1, RED);
  Vector3 center{0, 0, 0};
  Vector3 Vx{1, 0, 0};
  Vector3 minusVx{-1, 0, 0};

  Field Bfield_1{};
  Field Bfield_2{};

  // Note that in raylib the up coordinate is y
  for (std::size_t x = 0; x < static_cast<std::size_t>(wavePoints); ++x) {
    for (std::size_t y = 0; y < static_cast<std::size_t>(wavePoints); ++y) {
      for (std::size_t z = 0; z < static_cast<std::size_t>(wavePoints); ++z) {
        Bfield_1.Bfield[x][y][z] = getBfield(x, y, z, minusVx, length, 3);
      }
    }
  }
  // Note that in raylib the up coordinate is y
  for (std::size_t x = 0; x < static_cast<std::size_t>(wavePoints); ++x) {
    for (std::size_t y = 0; y < static_cast<std::size_t>(wavePoints); ++y) {
      for (std::size_t z = 0; z < static_cast<std::size_t>(wavePoints); ++z) {
        Bfield_2.Bfield[x][y][z] = getBfield(x, y, z, Vx, length, -3);
      }
    }
  }

  Field resultantB{sumFields(Bfield_1, Bfield_2)};

  while (!WindowShouldClose()) {
    // Update
    //----------------------------------------------------------------------------------
    UpdateCamera(&camera, CAMERA_FREE);

    if (IsKeyPressed(KEY_Z))
      camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    //----------------------------------------------------------------------------------

    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode3D(camera);
    double time = GetTime();

    // Note that in raylib the up coordinate is y
    for (std::size_t x = 0; x < static_cast<std::size_t>(wavePoints); ++x) {
      for (std::size_t y = 0; y < static_cast<std::size_t>(wavePoints); ++y) {
        for (std::size_t z = 0; z < static_cast<std::size_t>(wavePoints); ++z) {

          Vector3 endArrow = Vector3Normalize(resultantB.Bfield[x][y][z]);
          // Vector3 endArrow = Bfield_2.Bfield[x][y][z];
          // Finally  draw arrow only for some x
          if (BOARD[x][y][z].x == 0) {
            DrawLine3D(BOARD[x][y][z], Vector3Add(BOARD[x][y][z], endArrow),
                       GREEN);
            // transformArrowHead(x, y, z, Vx, endArrow, arrowModel, 3);
            // DrawModel(arrowModel, endArrow, 3.0f, WHITE);
          }
        }
      }
    }

    // Draw charge and its velocity vector
    Matrix cylinderRotationMatrix = MatrixRotateZ(-0.5f * PI);
    cylinderModel.transform = cylinderRotationMatrix;
    DrawModel(cylinderModel, {-static_cast<float>(xRange), 3, 0}, 1.0f, WHITE);
    DrawModel(cylinderModel, {-static_cast<float>(xRange), -3, 0}, 1.0f, WHITE);

    EndMode3D();
    DrawFPS(10, 10);
    EndDrawing();
  }

  // Unload models data (GPU VRAM)
  UnloadModel(cylinderModel);
  UnloadModel(arrowModel);
  CloseWindow();
  return 0;
}
