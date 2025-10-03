#include "simulation.h"

void Simulation::Draw() { grid.Draw(); }

void Simulation::SetCellValue(int row, int column, int value) {
  grid.setValue(row, column, value);
}
