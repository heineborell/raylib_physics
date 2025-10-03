#include "grid.h"
#include "simulation.h"
#include <raylib.h>

int main() {
  Color GREY{29, 29, 29, 255};
  const int screenWidth{750};
  const int screenHeight{750};
  const int FPS{12};
  const int cellSize{30};

  InitWindow(screenWidth, screenHeight, "Conway's Game of Life");
  SetTargetFPS(FPS);
  Simulation simulation(screenWidth, screenHeight, cellSize);
  simulation.SetCellValue(0, 2, 1);

  // Simulation Loop
  while (!WindowShouldClose()) {
    // 1- Event Handling

    // 2- Updating State

    // 3- Drawing
    BeginDrawing();
    ClearBackground(
        GREY); // clearing the background to grey before loop is run again
    simulation.Draw();
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
