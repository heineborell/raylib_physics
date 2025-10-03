#include "raylib.h"
#include <stdlib.h>
#include <string.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRID_WIDTH 200
#define GRID_HEIGHT 150
#define CELL_SIZE 4

// Particle types
typedef enum { EMPTY = 0, SAND, WATER, STONE, MUD } ParticleType;

// Colors for each particle type
Color particleColors[] = {
    {0, 0, 0, 0},         // EMPTY (transparent)
    {194, 178, 128, 255}, // SAND (sandy color)
    {64, 164, 223, 255},  // WATER (blue)
    {128, 128, 128, 255}, // STONE (gray)
    {139, 101, 48, 255}   // MUD (dark brown)
};

// Grid to store particles
ParticleType grid[GRID_HEIGHT][GRID_WIDTH];
ParticleType newGrid[GRID_HEIGHT][GRID_WIDTH];

// Current brush type
ParticleType currentBrush = SAND;
int brushSize = 2;

// Function to check if coordinates are valid
bool IsValidCell(int x, int y) {
  return x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT;
}

// Function to get random number between min and max
int GetRandomRange(int min, int max) { return min + rand() % (max - min + 1); }

// Function to clamp a value between min and max
int Clamp(int value, int min, int max) {
  if (value < min)
    return min;
  if (value > max)
    return max;
  return value;
}

// Function to check for mixing and create mud
bool TryCreateMud(int x, int y, ParticleType currentType) {
  // Check all 8 adjacent cells for mixing opportunities
  int directions[][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                         {0, 1},   {1, -1}, {1, 0},  {1, 1}};

  for (int i = 0; i < 8; i++) {
    int nx = x + directions[i][0];
    int ny = y + directions[i][1];

    if (IsValidCell(nx, ny)) {
      ParticleType neighborType = grid[ny][nx];

      // If sand is next to water or water is next to sand, create mud
      if ((currentType == SAND && neighborType == WATER) ||
          (currentType == WATER && neighborType == SAND)) {

        // 30% chance to create mud when they're adjacent
        if (GetRandomRange(1, 100) <= 30) {
          newGrid[y][x] = MUD;
          return true;
        }
      }
    }
  }
  return false;
}
// Clear both grids
memset(grid, EMPTY, sizeof(grid));
memset(newGrid, EMPTY, sizeof(newGrid));

// Add some ground (stone)
for (int x = 0; x < GRID_WIDTH; x++) {
  for (int y = GRID_HEIGHT - 5; y < GRID_HEIGHT; y++) {
    grid[y][x] = STONE;
  }
}

// Add some random stone obstacles
for (int i = 0; i < 20; i++) {
  int x = GetRandomRange(10, GRID_WIDTH - 10);
  int y = GetRandomRange(GRID_HEIGHT / 2, GRID_HEIGHT - 10);
  int size = GetRandomRange(2, 5);

  for (int dy = 0; dy < size; dy++) {
    for (int dx = 0; dx < size; dx++) {
      if (IsValidCell(x + dx, y + dy)) {
        grid[y + dy][x + dx] = STONE;
      }
    }
  }
}
}

// Initialize the simulation
void InitSimulation() {
  // Sand falls down if there's empty space
  if (IsValidCell(x, y + 1) && grid[y + 1][x] == EMPTY) {
    newGrid[y + 1][x] = SAND;
    return;
  }

  // If sand can't fall straight down, try to fall diagonally
  int direction = (rand() % 2) ? -1 : 1;

  // Try one diagonal direction first
  if (IsValidCell(x + direction, y + 1) &&
      grid[y + 1][x + direction] == EMPTY) {
    newGrid[y + 1][x + direction] = SAND;
    return;
  }

  // Try the other diagonal direction
  if (IsValidCell(x - direction, y + 1) &&
      grid[y + 1][x - direction] == EMPTY) {
    newGrid[y + 1][x - direction] = SAND;
    return;
  }

  // If can't move, stay in place
  newGrid[y][x] = SAND;
}

// Update water physics
void UpdateWater(int x, int y) {
  // Check if water should turn into mud when mixed with sand
  if (TryCreateMud(x, y, WATER)) {
    return;
  }

  // Water falls down if there's empty space
  if (IsValidCell(x, y + 1) && grid[y + 1][x] == EMPTY) {
    newGrid[y + 1][x] = WATER;
    return;
  }

  // Water can also displace sand
  if (IsValidCell(x, y + 1) && grid[y + 1][x] == SAND) {
    newGrid[y + 1][x] = WATER;
    newGrid[y][x] = SAND;
    return;
  }

  // If can't fall, try to move horizontally
  int direction = (rand() % 2) ? -1 : 1;

  // Try moving horizontally
  for (int i = 1; i <= 3; i++) {
    if (IsValidCell(x + direction * i, y) &&
        grid[y][x + direction * i] == EMPTY) {
      newGrid[y][x + direction * i] = WATER;
      return;
    }
    if (IsValidCell(x - direction * i, y) &&
        grid[y][x - direction * i] == EMPTY) {
      newGrid[y][x - direction * i] = WATER;
      return;
    }
  }

  // If can't move horizontally, try diagonally down
  if (IsValidCell(x + direction, y + 1) &&
      grid[y + 1][x + direction] == EMPTY) {
    newGrid[y + 1][x + direction] = WATER;
    return;
  }

  if (IsValidCell(x - direction, y + 1) &&
      grid[y + 1][x - direction] == EMPTY) {
    newGrid[y + 1][x - direction] = WATER;
    return;
  }

  // If can't move anywhere, stay in place
  newGrid[y][x] = WATER;
}

// Update mud physics
void UpdateMud(int x, int y) {
  // Mud behaves like thick sand - falls slower and is more viscous

  // Only 60% chance to fall each frame (slower than sand)
  if (GetRandomRange(1, 100) > 60) {
    newGrid[y][x] = MUD;
    return;
  }

  // Mud falls down if there's empty space
  if (IsValidCell(x, y + 1) && grid[y + 1][x] == EMPTY) {
    newGrid[y + 1][x] = MUD;
    return;
  }

  // Mud can displace water (but not sand)
  if (IsValidCell(x, y + 1) && grid[y + 1][x] == WATER) {
    newGrid[y + 1][x] = MUD;
    newGrid[y][x] = WATER;
    return;
  }

  // If mud can't fall straight down, try to fall diagonally (slower than sand)
  if (GetRandomRange(1, 100) <= 40) { // 40% chance to slide
    int direction = (rand() % 2) ? -1 : 1;

    // Try one diagonal direction first
    if (IsValidCell(x + direction, y + 1) &&
        grid[y + 1][x + direction] == EMPTY) {
      newGrid[y + 1][x + direction] = MUD;
      return;
    }

    // Try the other diagonal direction
    if (IsValidCell(x - direction, y + 1) &&
        grid[y + 1][x - direction] == EMPTY) {
      newGrid[y + 1][x - direction] = MUD;
      return;
    }
  }

  // If can't move, stay in place
  newGrid[y][x] = MUD;
}
// Clear new grid
memset(newGrid, EMPTY, sizeof(newGrid));

// Process particles from bottom to top, right to left (randomize order)
for (int y = GRID_HEIGHT - 1; y >= 0; y--) {
  // Randomize horizontal processing order
  int startX = rand() % 2 ? 0 : GRID_WIDTH - 1;
  int endX = startX == 0 ? GRID_WIDTH : -1;
  int stepX = startX == 0 ? 1 : -1;

  for (int x = startX; x != endX; x += stepX) {
    ParticleType particle = grid[y][x];

    switch (particle) {
    case SAND:
      UpdateSand(x, y);
      break;
    case WATER:
      UpdateWater(x, y);
      break;
    case MUD:
      UpdateMud(x, y);
      break;
    case STONE:
      newGrid[y][x] = STONE;
      break;
    case EMPTY:
      // Already empty in newGrid
      break;
    }
  }
}

// Copy new grid to current grid
memcpy(grid, newGrid, sizeof(grid));
}

// Update simulation
void UpdateSimulation() {
  int gridX = mouseX / CELL_SIZE;
  int gridY = mouseY / CELL_SIZE;

  for (int dy = -brushSize; dy <= brushSize; dy++) {
    for (int dx = -brushSize; dx <= brushSize; dx++) {
      if (dx * dx + dy * dy <= brushSize * brushSize) {
        int x = gridX + dx;
        int y = gridY + dy;

        if (IsValidCell(x, y) && grid[y][x] == EMPTY) {
          grid[y][x] = currentBrush;
        }
      }
    }
  }
}

// Add particles at mouse position
void AddParticles(int mouseX, int mouseY) {
  for (int y = 0; y < GRID_HEIGHT; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      ParticleType particle = grid[y][x];
      if (particle != EMPTY) {
        Color color = particleColors[particle];

        // Add some color variation for sand, water, and mud
        if (particle == SAND) {
          int variation = GetRandomRange(-20, 20);
          color.r = (unsigned char)Clamp(color.r + variation, 0, 255);
          color.g = (unsigned char)Clamp(color.g + variation, 0, 255);
          color.b = (unsigned char)Clamp(color.b + variation, 0, 255);
        } else if (particle == MUD) {
          int variation = GetRandomRange(-15, 15);
          color.r = (unsigned char)Clamp(color.r + variation, 0, 255);
          color.g = (unsigned char)Clamp(color.g + variation, 0, 255);
          color.b = (unsigned char)Clamp(color.b + variation, 0, 255);
        }

        DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE,
                      color);
      }
    }
  }
}

// Draw UI
void DrawUI() {
  // Draw instructions
  DrawText("Sand Simulator", 10, 10, 20, WHITE);
  DrawText("Left Click: Place particles", 10, 40, 16, WHITE);
  DrawText("Right Click: Erase", 10, 60, 16, WHITE);
  DrawText("1: Sand  2: Water  3: Stone  4: Mud", 10, 80, 16, WHITE);
  DrawText("Q/E: Change brush size", 10, 100, 16, WHITE);
  DrawText("R: Reset", 10, 120, 16, WHITE);

  // Draw current brush info
  const char *brushNames[] = {"Empty", "Sand", "Water", "Stone", "Mud"};
  DrawText(
      TextFormat("Brush: %s (Size: %d)", brushNames[currentBrush], brushSize),
      10, 150, 16, WHITE);

  // Draw brush preview at mouse position
  Vector2 mousePos = GetMousePosition();
  int gridX = (int)mousePos.x / CELL_SIZE;
  int gridY = (int)mousePos.y / CELL_SIZE;

  for (int dy = -brushSize; dy <= brushSize; dy++) {
    for (int dx = -brushSize; dx <= brushSize; dx++) {
      if (dx * dx + dy * dy <= brushSize * brushSize) {
        int x = gridX + dx;
        int y = gridY + dy;

        if (IsValidCell(x, y)) {
          DrawRectangleLines(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE,
                             WHITE);
        }
      }
    }
  }
}

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sand Simulator");
  SetTargetFPS(60);

  InitSimulation();

  while (!WindowShouldClose()) {
    // Handle input
    if (IsKeyPressed(KEY_ONE))
      currentBrush = SAND;
    if (IsKeyPressed(KEY_TWO))
      currentBrush = WATER;
    if (IsKeyPressed(KEY_THREE))
      currentBrush = STONE;
    if (IsKeyPressed(KEY_FOUR))
      currentBrush = MUD;

    if (IsKeyPressed(KEY_Q) && brushSize > 1)
      brushSize--;
    if (IsKeyPressed(KEY_E) && brushSize < 10)
      brushSize++;

    if (IsKeyPressed(KEY_R))
      InitSimulation();

    // Handle mouse input
    Vector2 mousePos = GetMousePosition();

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
      AddParticles((int)mousePos.x, (int)mousePos.y);
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
      int gridX = (int)mousePos.x / CELL_SIZE;
      int gridY = (int)mousePos.y / CELL_SIZE;

      for (int dy = -brushSize; dy <= brushSize; dy++) {
        for (int dx = -brushSize; dx <= brushSize; dx++) {
          if (dx * dx + dy * dy <= brushSize * brushSize) {
            int x = gridX + dx;
            int y = gridY + dy;

            if (IsValidCell(x, y)) {
              grid[y][x] = EMPTY;
            }
          }
        }
      }
    }

    // Update simulation
    UpdateSimulation();

    // Draw everything
    BeginDrawing();
    ClearBackground(BLACK);

    DrawSimulation();
    DrawUI();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
