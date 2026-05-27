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

constexpr std::size_t particleNumber{100};
constexpr double dt{0.001};
constexpr double totalT{10.0};
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
void plotter(int dimX, float L, std::vector<float> &resultFloatX, std::size_t i,
             float xRange, Color color) {
  for (std::size_t t{0}; t < dimT - 1; ++t) {

    const std::size_t offset = i * dimT + t;
    const std::size_t stride = dimT * particleNumber;

    // TODO: make this bob thing independent of number of bobs

    // first bob
    float x1{L * sin(resultFloatX[0 * stride + offset])};     // current x value
    float x2{L * sin(resultFloatX[0 * stride + offset + 1])}; // next x value
    float y1{-L * cos(resultFloatX[0 * stride + offset])};    // current y value
    float y2{-L * cos(resultFloatX[0 * stride + offset + 1])}; // next value

    // second bob
    float x12{x1 +
              L * sin(resultFloatX[1 * stride + offset])}; // current x value
    float x22{x2 +
              L * sin(resultFloatX[1 * stride + offset + 1])}; // next x value
    float y12{y1 -
              L * cos(resultFloatX[1 * stride + offset])}; // current y value
    float y22{y2 -
              L * cos(resultFloatX[1 * stride + offset + 1])}; // next value
    mapper(x1, y1, x2, y2, xRange, GREEN);
    mapper(x12, y12, x22, y22, xRange, GREEN);
  }
}

int main() {

  // solver piece

  std::vector<double> X(arraySize, 0.0);

  std::vector<std::function<double(double, double, double, double)>> rhs;

  // functions  to be integrated (rhs)
  double L{2};

  rhs.push_back(
      [](double theta1, double theta2, double x1, double x2) { return x1; });
  rhs.push_back(
      [](double theta1, double theta2, double x1, double x2) { return x2; });
  rhs.push_back([L](double theta1, double theta2, double x1, double x2) {
    return -1 / L * (2 * theta1 - theta2);
  });
  rhs.push_back([L](double theta1, double theta2, double x1, double x2) {
    return -1 / L * (-2 * theta1 + 2 * theta2);
  });

  // create thetas and its derivatives and set initial value (this is for
  // rungeKutta4OrderCpu)
  for (int i{0}; i < particleNumber; ++i) {
    double th1{0.01};
    double th2{0.02};
    double x1{0.2};
    double x2{0.1};
    X[0 * particleNumber * dimT + i] = th1; // theta1 initial
    X[1 * particleNumber * dimT + i] = th2; // theta2 initial
    X[2 * particleNumber * dimT + i] = x1;  // x1 initial
    X[3 * particleNumber * dimT + i] = x2;  // x2 initial
  }

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
  // showMatrix(X, particleNumber * dimT, dimY);
  std::vector<float> farray{castArray(X)};

  // plotting range
  float xRange{10.0f};

  InitWindow(screenWidth, screenHeight, "X-Y plot");
  SetTargetFPS(60);
  const float zoomSpeed{1.1f};

  while (!WindowShouldClose()) {
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
    for (std::size_t i{0}; i < particleNumber; ++i)
      plotter(dimT, L, farray, i, xRange, GREEN);

    EndDrawing();
  }
  return 0;
}
