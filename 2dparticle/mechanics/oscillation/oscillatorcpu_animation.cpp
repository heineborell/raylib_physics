#include "Random.h"
#include "helper.h"
#include "math.h"
#include "raylib.h"
#include <array>
#include <cstddef>
#include <functional>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>

const int screenWidth{980};
const int screenHeight{600};

constexpr std::size_t particleNoRoot{20};
constexpr std::size_t particleNumber{particleNoRoot * particleNoRoot};
constexpr double dt{0.001};
constexpr double totalT{100.0};
constexpr std::size_t dimT{static_cast<std::size_t>(totalT / dt)};
constexpr std::size_t dimY{4};
constexpr std::size_t arraySize{particleNumber * dimT * dimY};

void printArray(std::vector<double> const &arr) {
  for (double const &item : arr) {
    std::cout << item << '\n';
  }
}

std::vector<float> castArray(std::vector<double> &arr) {
  std::size_t N{arr.size()};
  std::vector<float> A(N, 0.0f);
  for (std::size_t i{0}; i < N; ++i) {
    A[i] = static_cast<float>(arr[i]);
  }
  return A;
}
std::vector<double> vectorScale(std::vector<double> &arr, double scalar) {
  std::vector<double> scaled;
  scaled.reserve(size(arr));
  for (double &item : arr)
    scaled.push_back(scalar * item);
  return scaled;
}

void showMatrix(std::vector<double> &vec, int gridDimX, int gridDimY) {
  int N{static_cast<int>(vec.size())};
  std::cout << std::setprecision(3);
  for (int i{0}; i < N; ++i) {
    if (i % gridDimX == 0)
      std::cout << '\n';
    std::cout << vec[i] << " ";
  }
}

// supposed to be faster on cpu multithread
// [x_1to x_1t1 ....... x2_t0 x_2_t1...]
void rungeKutta4OrderCpu(
    std::vector<double> &X,
    std::vector<std::function<double(double, double, double, double)>> &rhs,
    std::size_t i, double dt) {

  std::array<double, 4> X1{};
  std::array<double, 4> X2{};
  std::array<double, 4> X3{};
  std::array<double, 4> X4{};
  for (std::size_t t{0}; t < dimT - 1; ++t) {

    const std::size_t offset = i * dimT + t;
    const std::size_t stride = dimT * particleNumber;

    const double valtheta1 = X[0 * stride + offset];
    const double valtheta2 = X[1 * stride + offset];
    const double valx1 = X[2 * stride + offset];
    const double valx2 = X[3 * stride + offset];

    for (std::size_t y{0}; y < dimY; ++y) {
      X1[y] = rhs[y](valtheta1, valtheta2, valx1, valx2);
    } // f1
    for (std::size_t y{0}; y < dimY; ++y) {
      X2[y] = rhs[y](valtheta1 + dt / 2 * X1[0], valtheta2 + dt / 2 * X1[1],
                     valx1 + dt / 2 * X1[2], valx2 + dt / 2 * X1[3]);
    } // f2
    for (std::size_t y{0}; y < dimY; ++y) {
      X3[y] = rhs[y](valtheta1 + dt / 2 * X2[0], valtheta2 + dt / 2 * X2[1],
                     valx1 + dt / 2 * X2[2], valx2 + dt / 2 * X2[3]);
    } // f3
    for (std::size_t y{0}; y < dimY; ++y) {
      X4[y] = rhs[y](valtheta1 + dt / 2 * X3[0], valtheta2 + dt / 2 * X3[1],
                     valx1 + dt / 2 * X3[2], valx2 + dt / 2 * X3[3]);
    } // f4

    for (std::size_t y{0}; y < dimY; ++y) {
      X[y * stride + (dimT * i + (t + 1))] =
          X[y * stride + offset] +
          dt / 6 * (X1[y] + 2 * X2[y] + 2 * X3[y] + +X4[y]);
    } // averaging
  }
}

void mapper(float x1, float y1, float x2, float y2, float xRange, Color color) {

  // Map x and y values to screen coordinates
  Vector2 start = {screenWidth / 2 + x1 * (screenWidth / (2 * xRange)),
                   screenHeight / 2 - y1 * (screenHeight / (2 * xRange))};
  Vector2 end = {screenWidth / 2 + x2 * (screenWidth / (2 * xRange)),
                 screenHeight / 2 - y2 * (screenHeight / (2 * xRange))};
  DrawLineEx(start, end, 1.5f, color);
}

// Projecting to screen coordinates
Vector2 projectedVector(const Vector2 &vec, float xRange) {
  float width{static_cast<float>(screenWidth)};
  float height{static_cast<float>(screenHeight)};
  Vector2 projected = {
      static_cast<float>(width / 2 + vec.x * (width / (2 * xRange))),
      static_cast<float>(height / 2 - vec.y * (height / (2 * xRange)))};
  return projected;
}

void DrawTexturedCircle(Texture2D &tex, Vector2 &pos, float radius,
                        float rotation, float order, Color const &color) {
  Rectangle src = {order * 32, 0, 32, 32};

  Rectangle dst = {pos.x, pos.y, radius * 2.0f, radius * 2.0f};

  Vector2 origin = {radius, radius};

  DrawTexturePro(tex, src, dst, origin, rotation, color);
}

void plotter(double L, Texture2D &tex, std::vector<double> &X, int t,
             std::size_t i, int &j, float xRange) {

  std::size_t k{i % particleNoRoot};
  if (k == 0)
    ++j;
  const std::size_t offset{i * dimT + t};
  const std::size_t stride{dimT * particleNumber};

  // first bob
  double x1{L * sin(X[0 * stride + offset])};
  double y1{-L * cos(X[0 * stride + offset])};
  Vector2 first{
      static_cast<float>(-xRange + x1 + k * (2 * xRange / particleNoRoot)),
      static_cast<float>(-xRange + (2 * xRange / particleNoRoot) * (j + 1) +
                         y1)};
  Vector2 projectedfirst{projectedVector(first, xRange)};

  // second bob
  double x2{x1 + L * sin(X[1 * stride + offset])};
  double y2{y1 - L * cos(X[1 * stride + offset])};
  Vector2 second{
      static_cast<float>(-xRange + x2 + k * (2 * xRange / particleNoRoot)),
      static_cast<float>(-xRange + (2 * xRange / particleNoRoot) * (j + 1) +
                         y2)};
  Vector2 projectedsecond{projectedVector(second, xRange)};
  Vector2 projectedorigin{projectedVector(
      {static_cast<float>(-xRange + k * (2 * xRange / particleNoRoot)),
       static_cast<float>(-xRange + (2 * xRange / particleNoRoot) * (j + 1))},
      xRange)};

  // DrawTexturedCircle(tex, projectedfirst, 0.5, 0, 0, WHITE);
  // DrawTexturedCircle(tex, projectedsecond, 0.5, 0, 0, WHITE);
  DrawLineEx(projectedfirst, projectedsecond, 1.0f, GREEN);
  DrawLineEx(projectedorigin, projectedfirst, 1.0f, GREEN);
}

int main() {

  // solver piece

  std::vector<double> X(arraySize, 0.0);

  std::vector<std::function<double(double, double, double, double)>> rhs;

  // plotting range
  float xRange{10.0f};

  // functions  to be integrated (rhs)
  double L{xRange / particleNoRoot}; // length of pendulums
  double g{1};                       // gravitational const

  rhs.push_back(
      [](double theta1, double theta2, double x1, double x2) { return x1; });
  rhs.push_back(
      [](double theta1, double theta2, double x1, double x2) { return x2; });
  rhs.push_back([L, g](double theta1, double theta2, double x1, double x2) {
    double delta = theta1 - theta2;
    double den = L * (2.0 - cos(delta) * cos(delta));

    double num = g * (sin(theta2) * cos(delta) - 2.0 * sin(theta1)) -
                 L * (x2 * x2 + x1 * x1 * cos(delta)) * sin(delta);
    return num / den;
  });
  rhs.push_back([L, g](double theta1, double theta2, double x1, double x2) {
    double delta = theta1 - theta2;
    double den = L * (2.0 - cos(delta) * cos(delta));

    double num = 2.0 * g * (sin(theta1) * cos(delta) - sin(theta2)) +
                 L * (2.0 * x1 * x1 + x2 * x2 * cos(delta)) * sin(delta);
    return num / den;
  });

  // create thetas and its derivatives and set initial value (this is for
  // rungeKutta4OrderCpu)
  int j{0};
  for (int i{0}; i < particleNumber; ++i) {
    double th1{PI * (i % particleNoRoot) / particleNoRoot};
    if (i % particleNoRoot == 0) {
      ++j;
    }
    double th2{PI * j / particleNoRoot};
    double x1{0.2};
    double x2{0.2};
    X[0 * particleNumber * dimT + i * dimT] = th1; // theta1 initial
    X[1 * particleNumber * dimT + i * dimT] = th2; // theta2 initial
    X[2 * particleNumber * dimT + i * dimT] = x1;  // x1 initial
    X[3 * particleNumber * dimT + i * dimT] = x2;  // x2 initial
  }
  // showMatrix(X, particleNumber * dimT, dimY);

  // // Launch threads
  Timer timeCpu;
  std::vector<std::thread> threads;
  for (int i{0}; i < particleNumber; ++i) {
    threads.push_back(
        std::thread(rungeKutta4OrderCpu, std::ref(X), std::ref(rhs), i, dt));
  }
  // Join threads before program execution terminates
  for (auto &th : threads) {
    th.join();
  }

  std::cout << timeCpu.elapsed() << " seconds elapsed for the first solver."
            << '\n';
  std::vector<float> farray{castArray(X)};

  InitWindow(screenWidth, screenHeight, "X-Y plot");
  SetTargetFPS(60);
  const float zoomSpeed{1.1f};

  // Load textures
  std::vector<Rectangle> textureGrid{};
  Texture2D atlas =
      LoadTexture("../../../assets/colored_ball_shadow-Sheet.png");
  textureGrid.push_back(Rectangle{0 * 32, 0, 32, 32});
  textureGrid.push_back(Rectangle{1 * 32, 0, 32, 32});
  Texture2D circleTex = LoadTexture("../../../assets/face.png");

  int currentFrame{0};
  while (!WindowShouldClose()) {
    if (currentFrame < dimT - 1) {
      currentFrame = currentFrame + 50;
    } else {
      currentFrame = 0; // Loop the animation back to the beginning
    }
    if (IsKeyPressed(KEY_UP))
      xRange /= zoomSpeed; // zoom in
    if (IsKeyPressed(KEY_DOWN))
      xRange *= zoomSpeed; // zoom in
    if (xRange < 0.1f)
      xRange = 0.1f;
    if (xRange > 50.0f)
      xRange = 50.0f;

    int dimX{10};
    float step{xRange / dimX}; // step size for plotting

    BeginDrawing();
    ClearBackground(BLACK);
    DrawFPS(10, 10);
    DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, GRAY);
    DrawLine(0, screenHeight / 2, screenWidth, screenHeight / 2, GRAY);

    DrawText("Y", screenWidth / 2 + 5, 5, 20, GRAY);
    DrawText("X", screenWidth - 20, screenHeight / 2 + 5, 20, GRAY);

    int j = 0;
    for (std::size_t i{0}; i < particleNumber; ++i)
      plotter(L, circleTex, X, currentFrame, i, j, xRange);

    EndDrawing();
  }
  return 0;
}
