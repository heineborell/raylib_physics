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

inline constexpr int wavePoints{20};
inline constexpr double zoomSpeed{1.1};
inline constexpr double arrowAngle{3.14 / 7};
inline const double arrowSin{std::sin(arrowAngle)};
inline const double arrowCos{std::cos(arrowAngle)};

// Board

struct Field {
  vector<vector<Vector2>> Efield;
  std::vector<std::vector<double>> magnitudes;
};

struct Coordinates {
  double x;
  double y;
};

struct Magnitudes {
  double max;
  double min;
};

struct rgbValues {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
};

extern vector<vector<Vector2>> BOARD;
extern vector<vector<Vector2>> projectedBOARD;

double exFunc(double &x0, double &y0, double x, double distance_y);
double eyFunc(double &x0, double &y0, double x, double distance_y);

double xCompIntegrate(double &x0, double &y0, double y_pos,
                      double (*exFunc)(double &, double &, double, double));
double yCompIntegrate(double &x0, double &y0, double y_pos,
                      double (*eyFunc)(double &, double &, double, double));

Field getEfield(double charge_position);

void drawEfield(Field &efield, std::vector<rgbValues> &colors, double length,
                double xRange);
double map_to(double minimum, double maximum, double new_min, double new_max,
              double value);
double getMax(std::vector<vector<Vector2>> &BOARD, int COLS, int ROWS);
rgbValues getColorValue(double value, double minVal, double maxVal,
                        const std::vector<rgbValues> &colors);
Magnitudes getMaxLength(std::vector<std::vector<double>> &array);
Vector2 projectedVector(double x, double y, double xRange);
Vector2 Rotate(Vector2 v, double angle);
