#include "RealVector.h"
#include "config.h"
#include "particle.h"
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <raylib.h>
#include <sys/types.h>
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
  // get the max length for coloring purposes
  double max_length{getMax(BOARD, COLS, ROWS)};

  // Particle creation
  std::vector<Particle> particles;
  for (std::size_t i{0}; i < NUM_PARTICLES; ++i) {
    Particle p{RealVector(static_cast<float>(rand() % WIDTH),
                          static_cast<float>(rand() % HEIGHT)),
               RealVector(0, 0), PARTICLE_MIN_SPEED,
               PARTICLE_MAX_SPEED}; // generates random positions for particles
    particles.push_back(p);
  }

  while (!WindowShouldClose()) {

    BeginDrawing();
    ClearBackground(WHITE);
    DrawFPS(10, 10);

    for (int y = 0; y < ROWS; ++y) {
      for (int x = 0; x < COLS; ++x) {
        double x_component{getQuadrant(x, y, HALF_CELLS, BOARD).x};
        double y_component{getQuadrant(x, y, HALF_CELLS, BOARD).y};
        double angle = field_func(x_component, y_component);
        BOARD[y][x].vec = RealVector(cos(angle) * length, sin(angle) * length);
        Vector2 end = {BOARD[y][x].start_point.x + BOARD[y][x].vec.x,
                       BOARD[y][x].start_point.y + BOARD[y][x].vec.y};
        double magnitude{
            std::abs(RealVector(x_component, y_component).getMag())};
        u_char c = map_to(0, max_length, 0, 255, magnitude);

        DrawLineEx(BOARD[y][x].start_point, end, 2, (Color){0, 0, c, 255});
      }
    }
    for (std::size_t i{0}; i < NUM_PARTICLES; ++i) {
      RealVector mv = BOARD[static_cast<int>(particles[i].pos.y / SCL)]
                           [static_cast<int>(particles[i].pos.x / SCL)]
                               .vec;
      particles[i].applyForce(mv);
      particles[i].update();
      particles[i].show();
    }
    // charged object drawing
    Vector2 charge_start = {300, 400};
    Vector2 charge_end = {500, 400};
    DrawLineEx(charge_start, charge_end, 4, RED);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
