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

  Mesh cylinderMesh = GenMeshCylinder(0.03f, 2.0f, 20);
  Model cylinderModel = LoadModelFromMesh(cylinderMesh);
  cylinderModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = MAROON;

  Mesh arrow = GenMeshCone(0.03f, 0.1f, 10);
  Model arrowModel = LoadModelFromMesh(arrow);
  arrowModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = MAROON;

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

    step = xRange * 2 / wavePoints;
    length = step / 2.75;
    // DrawGrid(40, 1);
    // Draw charge and its velocity
    DrawSphere({0, 0, 0}, 0.1, RED);
    Vector3 center{0, 0, 0};
    Vector3 Vx{1, 0, 0};
    DrawLine3D(center, Vx, RED);

    // Note that in raylib the up coordinate is y
    for (std::size_t x{0}; x < static_cast<std::size_t>(wavePoints); ++x) {
      for (std::size_t y{0}; y < static_cast<std::size_t>(wavePoints); ++y) {
        for (std::size_t z{0}; z < static_cast<std::size_t>(wavePoints); ++z) {
          Vector3 unitVec{(Vector3CrossProduct(Vx, BOARD[x][y][z]))};
          float distance{Vector3Length(unitVec)};
          float thetaX{Vector3Angle(unitVec, {1, 0, 0})};
          float thetaY{Vector3Angle(unitVec, {0, 1, 0})};
          float thetaZ{Vector3Angle(unitVec, {0, 0, 1})};
          Matrix arrowRotationMatrix =
              MatrixMultiply(MatrixMultiply(MatrixRotateY(thetaY * PI),
                                            MatrixRotateX(thetaX * PI)),
                             MatrixRotateZ(thetaZ * PI));
          arrowModel.transform = arrowRotationMatrix;
          Vector3 end = {
              static_cast<float>(BOARD[x][y][z].x + cosf(thetaX) * length),
              static_cast<float>(BOARD[x][y][z].y + cosf(thetaY) * length),
              static_cast<float>(BOARD[x][y][z].z + cosf(thetaZ) * length)};

          if (x == 8) {
            DrawLine3D(BOARD[x][y][z], end, GREEN);
            DrawModel(arrowModel, end, 1.0f, WHITE);
          }
          // if (x == 16 && y % 2 == 0 && z % 2 == 0)
          //   DrawLine3D(BOARD[x][y][z], end, GREEN);
          // DrawLine3D(BOARD[x][y][z], unitVec, GREEN);
          // if (x + 1 < wavePoints)
          //   DrawLine3D(
          //       BOARD[x][y][z],
          //       {BOARD[x][y][z].x + 0.2f, BOARD[x][y][z].y,
          //       BOARD[x][y][z].z}, GREEN);
        }
      }
    }

    // DrawLine3D({0, 0, 0}, {1, 0, 0}, RED);
    // DrawLine3D({0, 0, 0}, {0, 0, 1}, BLUE);

    //
    // for (std::size_t y{0}; y < static_cast<std::size_t>(wavePoints); ++y)
    // {
    //   for (std::size_t x{0}; x < static_cast<std::size_t>(wavePoints);
    //   ++x) {
    //     projectedBOARD[y][x] = {
    //         projectedVector(BOARD[y][x].x, BOARD[y][x].y, xRange)};
    //   }
    // }

    // Spaceship setup
    Matrix cylinderRotationMatrix = MatrixRotateZ(-0.5f * PI);
    cylinderModel.transform = cylinderRotationMatrix;
    DrawModel(cylinderModel, Vector3Zero(), 1.0f, WHITE);

    EndMode3D();
    DrawFPS(10, 10);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
