#pragma once
#include "grid.h"

class Simulation {
public:
  Simulation(int width, int height, int cellSize)
      : grid(width, height, cellSize) {};
  void Draw();
  void SetCellValue(int row, int column, int value);

private:
  Grid grid;
};
