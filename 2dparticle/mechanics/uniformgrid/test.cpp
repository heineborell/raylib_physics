#include "Random.h"
#include "config.h"
#include "particle.h"
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
  SetTargetFPS(80);
  Texture2D circleTex = LoadTexture("../../../assets/face.png");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturedCircle(circleTex, {WIDTH * 0.5f, HEIGHT * 0.5f}, 64.0f);

    DrawFPS(10, 10);
    EndDrawing();
  }

  // std::cout << "Thread no " << updateThread.get_id() << " closed!" << '\n';
  // updateThread.join();
  UnloadTexture(circleTex);
  CloseWindow();
  return 0;
}
