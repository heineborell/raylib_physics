#include "RealVector.h"
#include "raylib.h"
#include <vector>

using std::vector;

inline constexpr int WIDTH{800};
inline constexpr int HEIGHT{800};
inline constexpr int SCL{20}; // scale means the dimensions of grid cells
inline constexpr int ROWS{(HEIGHT / SCL)};
inline constexpr int COLS{(WIDTH / SCL)};
inline constexpr int FPS{60};

// Board

struct slot {
  RealVector vec;
  Vector2 start_point; // starting position where to draw the vector on screen
};

extern vector<vector<slot>> BOARD;

double map_to(double minimum, double maximum, double new_min, double new_max,
              double value);
double field_func(double x, double y);
