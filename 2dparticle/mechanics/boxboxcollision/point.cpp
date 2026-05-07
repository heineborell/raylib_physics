#include "Random.h"
#include "config.h"
#include "uniformgrid.h"
#include <cmath>
#include <cstddef>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <sys/types.h>
#include <thread>
#include <vector>

int main() {

  InitWindow(HEIGHT, WIDTH, "Particle trajectory plot");

  double xRange{20.0};
  double scaleX{WIDTH / (2 * xRange)};
  double scaleY{HEIGHT / (2 * xRange)}; // or xRange if square

  // Start Grid and Particles
  RealVector2 accelaration{0, 0};
  SpatialGrid grid{{{-xRange, -xRange}, {xRange, xRange}}, {NCELLS, NCELLS}};

  // Initial Conditions, masses
  double massSmall{1};
  double massBig{1E4};
  double sqrtMassSmall{std::sqrt(massSmall)};
  double sqrtMassBig{std::sqrt(massBig)};
  RealVector2 initialPositionSmall{-19, 2};
  RealVector2 initialVelocitySmall{0, 0};
  RealVector2 initialPositionBig{-13, 2};
  RealVector2 initialVelocityBig{-5000, 0};
  RealVector2 dimensionsSmall{1, 1};
  RealVector2 dimensionsBig{4, 4};

  Shape shape{Shape::ball};
  grid.newClient(initialPositionSmall, dimensionsSmall, initialVelocitySmall,
                 massSmall, shape);
  grid.newClient(initialPositionBig, dimensionsBig, initialVelocityBig, massBig,
                 shape);
  // Conserved quantities
  double speedSmall{initialVelocitySmall.x};
  double speedBig{initialVelocityBig.x};
  double scaledSpeedSmall{sqrtMassSmall * speedSmall};
  double scaledSpeedBig{sqrtMassBig * scaledSpeedBig};

  double energySmall{0.5 * (massBig * speedSmall * speedSmall)};
  double energyBig{0.5 * (massSmall * speedBig * speedBig)};
  double energy{energySmall + energyBig};
  // circle plot
  RealVector2 centerCircle{projectedVector({15.0, -15.0}, xRange)};
  double circleScale{8};
  std::vector<RealVector2> lines{};

  // Load textures
  std::vector<Rectangle> textureGrid{};
  Texture2D atlas = LoadTexture("../../../assets/ball_box.png");
  textureGrid.push_back(Rectangle{0 * 32, 0, 32, 32});
  textureGrid.push_back(Rectangle{1 * 32, 0, 32, 32});
  Texture2D circleTex = LoadTexture("../../../assets/face.png");

  // SetTargetFPS(FPS);
  while (isRunning) {
    if (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose())
      isRunning = false;

    if (IsKeyPressed(KEY_SPACE))
      ToggleFullscreen();

    BeginDrawing();
    ClearBackground(BLACK);
    dt = 0.00000001;
    grid.update();
    plotter(grid, atlas, xRange, scaleX);

    speedSmall = (grid.m_clients[0].m_velocity.x);
    speedBig = (grid.m_clients[1].m_velocity.x);
    energySmall = 0.5 * (massSmall * speedSmall * speedSmall);
    energyBig = 0.5 * (massBig * speedBig * speedBig);
    energy = energySmall + energyBig;

    // Drawing of circle and lines
    scaledSpeedSmall =
        circleScale * (sqrtMassSmall * speedSmall) / (std::sqrt(2 * energy));
    scaledSpeedBig =
        circleScale * (sqrtMassBig * speedBig) / (std::sqrt(2 * energy));
    RealVector2 scaledSpeeds{
        projectedVector(scaledSpeedBig, scaledSpeedSmall, xRange)};
    lines.push_back(scaledSpeeds);

    if (lines.size() > 1)
      for (size_t i{0}; i < lines.size(); ++i) {
        if (i == 0)
          continue;
        else
          DrawLineEx({200 + lines[i].x, 200 + lines[i].y},
                     {200 + lines[i - 1].x, 200 + lines[i - 1].y}, 1.0, RED);
      }

    DrawCircleLines(700, 700, circleScale * scaleX, GREEN);
    DrawCircle(200 + scaledSpeeds.x, 200 + scaledSpeeds.y, 3, RED);

    // Draw axes
    DrawFPS(10, 10);
    DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, GRAY);
    DrawLine(0, HEIGHT / 2, WIDTH, HEIGHT / 2, GRAY);

    DrawText("Y", WIDTH / 2 + 5, 5, 20, GRAY);
    DrawText("X", WIDTH - 20, HEIGHT / 2 + 5, 20, GRAY);

    DrawText(TextFormat("Number of Collisions:%i", static_cast<int>(counter)),
             10, 30, 40,
             YELLOW); // draw equation on text

    DrawText(
        TextFormat(
            "Energy of m1: %.2E, Energy of m2: %.2E, Total energy: : %.2E ",
            energySmall, energyBig, energy),
        10, 80, 20,
        BLUE); // draw equation on text

    EndDrawing();
  }

  UnloadTexture(circleTex);
  UnloadTexture(atlas);
  isRunning = false;
  CloseWindow();
  return 0;
}
