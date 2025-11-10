#include "config.h"
#include "rlgl.h"
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <sys/types.h>
#include <vector>

// I think this one is bloated can be written better
void DrawDashedLine3D(Vector3 start, Vector3 end, float dashLength,
                      float gapLength, Color color) {
  Vector3 direction = {end.x - start.x, end.y - start.y, end.z - start.z};
  float length = sqrt(direction.x * direction.x + direction.y * direction.y +
                      direction.z * direction.z);
  Vector3 unitDir = {direction.x / length, direction.y / length,
                     direction.z / length};

  float total = 0.0f;
  bool drawDash = true;
  Vector3 current = start;

  while (total < length) {
    float segment = drawDash ? dashLength : gapLength;
    if (total + segment > length)
      segment = length - total;

    if (drawDash) {
      Vector3 next = {current.x + unitDir.x * segment,
                      current.y + unitDir.y * segment,
                      current.z + unitDir.z * segment};
      DrawLine3D(current, next, color);
      current = next;
    } else {
      current.x += unitDir.x * segment;
      current.y += unitDir.y * segment;
      current.z += unitDir.z * segment;
    }

    total += segment;
    drawDash = !drawDash;
  }
}

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
  Mesh cylinderMesh = GenMeshCylinder(0.03f, 2.0f, 20);
  Model cylinderModel = LoadModelFromMesh(cylinderMesh);
  cylinderModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = MAROON;

  // Define cone mesh for the arrow Heads
  Mesh arrow = GenMeshCone(0.03f, 0.1f, 5);
  Model arrowModel = LoadModelFromMesh(arrow);
  arrowModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = GREEN;

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
    for (std::size_t x = 0; x < static_cast<std::size_t>(wavePoints); ++x) {
      for (std::size_t y = 0; y < static_cast<std::size_t>(wavePoints); ++y) {
        for (std::size_t z = 0; z < static_cast<std::size_t>(wavePoints); ++z) {

          // Compute unit vector of the B field B=vXr
          Vector3 unitVec =
              Vector3Normalize(Vector3CrossProduct(Vx, BOARD[x][y][z]));
          Vector3 unitR = Vector3Normalize(BOARD[x][y][z]);

          // This is for the arrow cylinder. First compute rotation to align +Y
          // with B field at each point. Then compute quaternion for rotation.
          // Finally transform
          Vector3 up = {0, 1, 0};
          Vector3 axis = Vector3CrossProduct(up, unitVec);
          float axisLength = Vector3Length(axis);
          float arrowAngle = acosf(Vector3DotProduct(up, unitVec));

          // Handle edge cases (parallel or anti-parallel vectors)
          if (axisLength < 0.0001f) {
            // when unitvec is nearly parallel or opposite to +Y
            if (unitVec.y < 0.0f) { // opposite
              std::cout << axisLength << '\n';
              axis = (Vector3){1, 0, 0};
              arrowAngle = PI;
            } else { // same direction
              axis = (Vector3){1, 0, 0};
              arrowAngle = 0;
            }
          }
          Quaternion q =
              QuaternionFromAxisAngle(Vector3Normalize(axis), arrowAngle);
          arrowModel.transform = QuaternionToMatrix(q);
          Vector3 endArrow =
              Vector3Add(BOARD[x][y][z], Vector3Scale(unitVec, length));

          // Finally  draw arrow only for x==8 as an example
          if (x == 8) {
            DrawLine3D(BOARD[x][y][z], endArrow, GREEN);
            DrawModel(arrowModel, endArrow, 3.0f, WHITE);
            if (IsKeyDown(KEY_G)) {
              if (y % 5 == 0 && z % 5 == 0) {
                DrawDashedLine3D(BOARD[x][y][z], {0, 0, 0}, 0.08, 0.1, BLUE);
                DrawLine3D(
                    BOARD[x][y][z],
                    {BOARD[x][y][z].x + 1, BOARD[x][y][z].y, BOARD[x][y][z].z},
                    MAROON);
                DrawLine3D(Vector3Add(BOARD[x][y][z], unitR * length),
                           BOARD[x][y][z], BLUE);
              }
            }
          }
        }
      }
    }

    // Draw charge and its velocity vector
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
