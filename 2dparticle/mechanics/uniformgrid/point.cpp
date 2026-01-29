#include "Random.h"
#include "config.h"
#include "uniformgrid.h"
#include <GL/gl.h>
#include <cmath>
#include <complex>
#include <cstddef>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <sys/types.h>
#include <thread>
#include <vector>

void DrawTexturedCircle(Texture2D tex, Vector2 pos, float radius) {
  Rectangle src = {0, 0, (float)tex.width, (float)tex.height};

  Rectangle dst = {pos.x, pos.y, radius * 2.0f, radius * 2.0f};

  Vector2 origin = {radius, radius};

  DrawTexturePro(tex, src, dst, origin, 0.0f, WHITE);
}

int main() {

  InitWindow(HEIGHT, WIDTH, "Particle trajectory plot");
  // SetTargetFPS(FPS);

  float xRange{4.0};
  float scaleX{WIDTH / (2 * xRange)};
  float scaleY{HEIGHT / (2 * xRange)}; // or xRange if square
  printf("Renderer: %s\n", glGetString(GL_RENDERER));
  printf("Vendor:   %s\n", glGetString(GL_VENDOR));

  // // create particles with random initial positions and velocities
  // std::vector<Particle> pparticles;
  // float speed{4.0f};
  //
  // for (int i{0}; i < NUM_PARTICLES; ++i) {
  //   if (i < NUM_PARTICLES / 2) {
  //     int angle{Random::get(1, 20)};
  //     Vector2 initialPosition{3, static_cast<float>(Random::get(-4, 4))};
  //     Vector2 initialVelocity{-speed * std::cos(PI / (2 * angle)),
  //                             speed * std::sin(PI / (2 * angle))};
  //     pparticles.push_back(Particle{initialPosition, initialVelocity});
  //   } else {
  //     int angle{Random::get(1, 20)};
  //     Vector2 initialPosition{-3, static_cast<float>(Random::get(-4, 4))};
  //     Vector2 initialVelocity{speed * std::cos(PI / (2 * angle)),
  //                             speed * std::sin(PI / (2 * angle))};
  //     pparticles.push_back(Particle{initialPosition, initialVelocity});
  //   }
  // }
  //
  Vector2 accelaration{0, 0};
  float dt{1.0f / 60.0f};
  //
  // std::thread updateThread(updater, std::ref(pparticles),
  //                          std::ref(accelaration), std::ref(dt), xRange);
  //
  SpatialGrid grid{{{-xRange, -xRange}, {xRange, xRange}}, {NCELLS, NCELLS}};
  for (int i{0}; i <= NUM_PARTICLES; ++i) {

    int angle{Random::get(1, 20)};
    float speed{static_cast<float>(Random::get(-3, 3))};
    Vector2 initialPosition{-3.9f + (Random::get(0, 7800) / 1000.0f),
                            -3.9f + (Random::get(0, 7800) / 1000.0f)};
    Vector2 initialVelocity{-speed * std::cos(PI / (2 * angle)),
                            speed * std::sin(PI / (2 * angle))};
    Vector2 dimensions{PARTICLE_RADIUS, PARTICLE_RADIUS};
    grid.newClient(initialPosition, dimensions, initialVelocity);
  }

  SetTargetFPS(60);
  std::cout << "size of each cell " << 2 * xRange / NCELLS << '\n';
  std::cout << "size of the grid " << grid.m_cells.size() << '\n';
  Texture2D circleTex = LoadTexture("../../../assets/face.png");

  while (isRunning) {
    if (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose())
      isRunning = false;

    if (IsKeyPressed(KEY_SPACE))
      ToggleFullscreen();

    grid.update(dt);
    BeginDrawing();
    ClearBackground(BLACK);

    // Draw clients
    for (auto &client : grid.m_clients) {
      // std::cout << client << '\n';
      Vector2 projectedClientpos{projectedVector(client.m_position, xRange)};
      DrawTexturedCircle(circleTex, projectedClientpos,
                         client.m_dimensions.x * scaleX * 0.5f);
      //            client.m_dimensions.x * scaleX * 0.5f, RED);
      // DrawCircle(projectedClientpos.x, projectedClientpos.y,
      //            client.m_dimensions.x * scaleX * 0.5f, RED);
    }
    // DrawRectangle(projectedClientpos.x - 1.0 * scaleX * 0.5f,
    //               projectedClientpos.y - 1.0 * scaleY * 0.5f, 1.0 * scaleX,
    //               1.0 * scaleY, RED);

    // Draw all hitbox rectangles
    // for (int i{0}; i < grid.m_cells.size(); ++i) {
    //   if (grid.m_cells[i].size() > 0) {
    //     for (auto &client : grid.m_cells[i]) {
    //       float cellW = WIDTH / (float)NCELLS;
    //       float cellH = HEIGHT / (float)NCELLS;
    //       int x = i % (NCELLS);
    //       int y = i / (NCELLS);
    //       DrawRectangle(x * cellW, y * cellH, cellW, cellH, {0, 228, 48,
    //       100});
    //     }
    //   }
    // }

    // Draw the ones that are m_nearby
    //  if (!grid.m_clients[0].m_nearby.empty()) {
    //    for (auto &client : grid.m_clients[0].m_nearby) {
    //      for (auto i : client->m_cellInfo) {
    //        float cellW = WIDTH / (float)NCELLS;
    //        float cellH = HEIGHT / (float)NCELLS;
    //        int x = i.cellNumber % (NCELLS);
    //        int y = i.cellNumber / (NCELLS);
    //        DrawRectangle(x * cellW, y * cellH, cellW, cellH, MAROON);
    //      }
    //    }
    //  }

    // plotter(pparticles, xRange);
    // std::size_t totalSize{0};
    // for (auto &cell : grid.m_cells) {
    //   std::size_t length{cell.size()};
    //   totalSize += length;
    // }
    // std::cout << totalSize << " size of total hitboxes " << '\n';
    // std::cout << grid.m_clients.size() << '\n';

    // std::cout << grid.m_clients[1].lastQueryId << '\n';
    DrawFPS(10, 10);
    EndDrawing();
  }

  // std::cout << "Thread no " << updateThread.get_id() << " closed!" << '\n';
  // updateThread.join();
  UnloadTexture(circleTex);
  isRunning = false;
  CloseWindow();
  return 0;
}
