#include "RealVector.h"
#include "config.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <raylib.h>
#include <time.h>

int main() {
  const int screenHeight{800};
  const int screenWidth{600};
  const int fps{60};

  InitWindow(screenHeight, screenHeight, "Vector PLots");
  srand(time(nullptr));
  SetTargetFPS(fps);

  double MULT{0.5f};
  double length{SCL * MULT}; // size of the vectors we draw
  for (int y = 0; y < ROWS; ++y) {
    for (int x = 0; x < COLS; ++x) {
      slot s;
      s.start_point = {(float)SCL * x + SCL / 2,
                       (float)SCL * y + SCL / 2}; // (float is C-style cast!)
      BOARD[y][x] = s;
    }
  }

  while (!WindowShouldClose()) {

    BeginDrawing();
    ClearBackground(WHITE);

    for (int y = 0; y < ROWS; ++y) {
      for (int x = 0; x < COLS; x++) {
        double angle = field_func(static_cast<double>(x) * 0.03,
                                  static_cast<double>(y) * 0.03);
        BOARD[y][x].vec = RealVector(cos(angle) * length, sin(angle) * length);
        Vector2 end = {BOARD[y][x].start_point.x + BOARD[y][x].vec.x,
                       BOARD[y][x].start_point.y + BOARD[y][x].vec.y};
        DrawLineEx(BOARD[y][x].start_point, end, 1, (Color){0, 0, 0, 255});
      }
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
