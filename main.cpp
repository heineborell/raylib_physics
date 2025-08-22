#include "raylib.h"
#include <vector>

int main() {
  InitWindow(800, 600, "2D Vectors as Points");

  SetTargetFPS(60);

  // Example list of 2D points (vectors)
  std::vector<Vector2> points = {
      {100, 100}, {200, 150}, {300, 250}, {400, 200}, {500, 350}};

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw all points
    for (auto &p : points) {
      // DrawCircleV takes a Vector2 directly
      DrawCircleV(p, 5, RED); // (point, radius, color)
    }

    // Draw coordinate axes
    DrawLine(0, 300, 800, 300, LIGHTGRAY); // X-axis
    DrawLine(400, 0, 400, 600, LIGHTGRAY); // Y-axis

    DrawText("2D vectors as points", 10, 10, 20, DARKGRAY);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
