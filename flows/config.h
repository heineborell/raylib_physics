#include "RealVector.h"
#include "raylib.h"
#include <vector>

using std::vector;

inline constexpr int WIDTH{800};
inline constexpr int HEIGHT{800};
inline constexpr int SCL{1}; // scale means the dimensions of grid cells
inline constexpr int ROWS{(HEIGHT / SCL)};
inline constexpr int COLS{(WIDTH / SCL)};
inline constexpr int HALF_CELLS{(ROWS / 2)};
inline constexpr int FPS{60};
inline constexpr double MULT{0.08f};
inline constexpr double length{SCL * MULT}; // size of the vectors we draw

inline constexpr int PARTICLE_RADIUS{2};
inline constexpr int PARTICLE_MAX_SPEED{2};
inline constexpr int PARTICLE_MIN_SPEED{1};
inline constexpr int NUM_PARTICLES{4000};

inline constexpr int wavePoints{40};
inline constexpr float zoomSpeed{1.1f};

// Board

struct slot {
  RealVector vec;
  Vector2 start_point; // starting position where to draw the vector on screen
};

struct Coordinates {
  double x;
  double y;
};

extern vector<vector<slot>> BOARD;

double xComponent(double x, double y);
double yComponent(double x, double y);
double map_to(double minimum, double maximum, double new_min, double new_max,
              double value);
double field_func(const double &x, const double &y);
double getMax(std::vector<vector<slot>> &BOARD, int COLS, int ROWS);
Coordinates getQuadrant(int x, int y, int half_no_cells,
                        std::vector<std::vector<slot>> &BOARD);
Vector2 projectedVector(float x, float y, float xRange);
