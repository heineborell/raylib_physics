#include "RealVector.h"
#include "config.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <raylib.h>
#include <time.h>

double xComponent(double x, double y);
double yComponent(double x, double y);

int main() {

  InitWindow(HEIGHT, WIDTH, "Vector PLots");
  srand(time(nullptr));
  SetTargetFPS(FPS);

  double MULT{0.8f};
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
        // std::cout << xComponent(BOARD[y][x].start_point.x,
        //                         BOARD[y][x].start_point.y)
        //           << '\n';
        std::cout << BOARD[y][x].start_point.y
                  << field_func(xComponent(BOARD[y][x].start_point.x,
                                           BOARD[y][x].start_point.y),
                                yComponent(BOARD[y][x].start_point.x,
                                           BOARD[y][x].start_point.y))
                  << '\n';

        // double angle = field_func(0.0, 1.0);
        double angle = field_func(
            xComponent(BOARD[y][x].start_point.x, BOARD[y][x].start_point.y),
            yComponent(BOARD[y][x].start_point.x, BOARD[y][x].start_point.y));
        BOARD[y][x].vec = RealVector(cos(angle) * length, sin(angle) * length);
        Vector2 end = {BOARD[y][x].start_point.x + BOARD[y][x].vec.x,
                       BOARD[y][x].start_point.y + BOARD[y][x].vec.y};
        DrawLineEx(BOARD[y][x].start_point, end, 2, (Color){0, 0, 0, 255});
      }
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
