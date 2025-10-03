#include "RealVector.h"
#include "raylib.h"
#include <vector>

using std::vector;

#define WIDTH 800
#define HEIGHT 800
#define SCL 10 // scale means the dimensions of grid cells
#define ROWS (HEIGHT / SCL)
#define COLS (WIDTH / SCL)

// Board

struct slot {
  RealVector vec;
  Vector2 start_point; // starting position where to draw the vector on screen
};

extern vector<vector<slot>> BOARD;

double map_to(double minimum, double maximum, double new_min, double new_max,
              double value);
double field_func(double x, double y);
