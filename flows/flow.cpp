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

int main() {

  InitWindow(HEIGHT, WIDTH, "Vector PLots");
  srand(time(nullptr));
  SetTargetFPS(FPS);

  float xRange{4.0f}; // x will range from -4 to 4 but then changed by scrolling
  float yRange{4.0f};
  float step{xRange * 2 / wavePoints}; // step size for plotting

  for (std::size_t y{0}; y < static_cast<std::size_t>(wavePoints); ++y) {
    for (std::size_t x{0}; x < static_cast<std::size_t>(wavePoints); ++x) {
      slot s;
      s.start_point = {
          static_cast<float>(-xRange + x * step),
          static_cast<float>(-yRange + y * step)}; // (float is C-style cast!)
      BOARD[y][x] = s;
    }
  }
  // for (std::size_t i{0}; i < wavePoints - 1; ++i) {
  //   float x1{-xRange + i * step}; // current x value
  //   float x2{x1 + step};          // next x value
  //   float y1{polynomialWave(x1, a, b, c, d, e, f, g)};
  //   float y2{polynomialWave(x2, a, b, c, d, e, f, g)};
  //
  //   // Map x and y values to screen coordinates
  //   Vector2 start = {WIDTH / 2 + x1 * (WIDTH / (2 * xRange)),
  //                    HEIGHT / 2 - y1 * (HEIGHT / (2 * xRange))};
  //   Vector2 end = {WIDTH / 2 + x2 * (WIDTH / (2 * xRange)),
  //                  HEIGHT / 2 - y2 * (HEIGHT / (2 * xRange))};
  //   DrawLineEx(start, end, 2.0f, WHITE);
  // }

  // get the max length for coloring purposes
  // double max_length{getMax(BOARD, COLS, ROWS)};

  // Particle creation
  // std::vector<Particle> particles;
  // for (std::size_t i{0}; i < NUM_PARTICLES; ++i) {
  //   Particle p{RealVector(static_cast<float>(rand() % WIDTH),
  //                         static_cast<float>(rand() % HEIGHT)),
  //              RealVector(0, 0), PARTICLE_MIN_SPEED,
  //              PARTICLE_MAX_SPEED}; // generates random positions for
  //              particles
  //   particles.push_back(p);
  // }

  while (!WindowShouldClose()) {

    BeginDrawing();
    ClearBackground(BLACK);
    DrawFPS(10, 10);

    if (IsKeyPressed(KEY_UP))
      xRange /= zoomSpeed; // zoom in
    if (IsKeyPressed(KEY_DOWN))
      xRange *= zoomSpeed; // zoom in
    if (xRange < 0.1f)
      xRange = 0.1f;
    if (xRange > 50.0f)
      xRange = 50.0f;

    // Draw axes
    DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, GRAY);
    DrawLine(0, HEIGHT / 2, WIDTH, HEIGHT / 2, GRAY);

    DrawText("Y", WIDTH / 2 + 5, 5, 20, GRAY);
    DrawText("X", WIDTH - 20, HEIGHT / 2 + 5, 20, GRAY);
    float step{xRange * 2 / wavePoints}; // step size for plotting

    std::cout << xRange << '\n';
    for (std::size_t y{0}; y < static_cast<std::size_t>(wavePoints); ++y) {
      for (std::size_t x{0}; x < static_cast<std::size_t>(wavePoints); ++x) {
        double x_component{0};
        double y_component{1};
        double angle = field_func(x_component, y_component);
        BOARD[y][x].vec = RealVector(cos(angle) * length, sin(angle) * length);
        Vector2 end = {BOARD[y][x].start_point.x + BOARD[y][x].vec.x,
                       BOARD[y][x].start_point.y + BOARD[y][x].vec.y};

        // // Map x and y values to screen coordinates
        Vector2 projectedStart{projectedVector(
            BOARD[y][x].start_point.x, BOARD[y][x].start_point.y, xRange)};
        Vector2 projectedEnd{projectedVector(end.x, end.y, xRange)};

        DrawLineEx(projectedStart, projectedEnd, 1, (Color){0, 255, 0, 255});
      }
    }

    // for (int y = 0; y < ROWS; ++y) {
    //   for (int x = 0; x < COLS; ++x) {
    //     double x_component{getQuadrant(x, y, HALF_CELLS, BOARD).x};
    //     double y_component{getQuadrant(x, y, HALF_CELLS, BOARD).y};
    //     double angle = field_func(x_component, y_component);
    //     BOARD[y][x].vec = RealVector(cos(angle) * length, sin(angle) *
    //     length);
    //     Vector2 end = {BOARD[y][x].start_point.x + BOARD[y][x].vec.x,
    //     BOARD[y][x].start_point.y + BOARD[y][x].vec.y}; double magnitude{
    //     std::abs(RealVector(x_component, y_component).getMag())};
    //     // u_char c = map_to(0, max_length, 0, 255, magnitude);
    //
    //     DrawLineEx(BOARD[y][x].start_point, end, 2, (Color){0, 255, 0, 255});
    //   }
    // }
    // for (std::size_t i{0}; i < NUM_PARTICLES; ++i) {
    //   RealVector mv = BOARD[static_cast<int>(particles[i].pos.y / SCL)]
    //                        [static_cast<int>(particles[i].pos.x / SCL)]
    //                            .vec;
    //   particles[i].applyForce(mv);
    //   particles[i].update();
    //   particles[i].show();
    // }
    // charged object drawing
    Vector2 charge_start = {projectedVector(-1, 0, xRange)};
    Vector2 charge_end = {projectedVector(1, 0, xRange)};
    DrawLineEx(charge_start, charge_end, 4, RED);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
