#include "RealVector.h"
#include "raylib.h"
#include <cmath>
#include <sys/types.h>
#include <vector>

using std::vector;

inline constexpr int WIDTH{1000};
inline constexpr int HEIGHT{1000};
inline constexpr int FPS{60};

inline constexpr int PARTICLE_RADIUS{2};
inline constexpr int PARTICLE_MAX_SPEED{2};
inline constexpr int PARTICLE_MIN_SPEED{1};
inline constexpr int NUM_PARTICLES{4000};

inline constexpr int wavePoints{40};
inline constexpr float zoomSpeed{1.1f};
inline constexpr float arrowAngle{3.14f / 7};

// Board

struct slot {
  Vector2 vec;
  Vector2 start_point; // starting position where to draw the vector on screen
};

struct Coordinates {
  float x;
  float y;
};

struct Magnitudes {
  float max;
  float min;
};

struct rgbValues {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
};

extern vector<vector<slot>> BOARD;

float xComponent(float x, float y);
float yComponent(float x, float y);
float map_to(float minimum, float maximum, float new_min, float new_max,
             float value);
float getMax(std::vector<vector<slot>> &BOARD, int COLS, int ROWS);
rgbValues getColorValue(float value, float minVal, float maxVal,
                        const std::vector<rgbValues> &colors);
Magnitudes getMaxLength(std::vector<std::vector<float>> &array);
Vector2 projectedVector(float x, float y, float xRange);
Vector2 Rotate(Vector2 v, float angle);
