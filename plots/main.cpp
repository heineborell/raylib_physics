#include "math.h"
#include "raylib.h"
#include "stdio.h"
#include <cstddef>

float polynomialWave(float x, float a, float b, float c, float d, float e,
                     float f, float g) {
  float y{static_cast<float>(a * pow(x, 6) + b * pow(x, 5) + c * pow(x, 4) +
                             d * pow(x, 3) + e * pow(x, 2) + f * x + g)};
  return y;
}

int main() {
  const int screenWidth{1000};
  const int screenHeight{640};
  InitWindow(screenWidth, screenHeight, "polynomialWave");
  SetTargetFPS(60);
  const int wavePoints{400};
  float xRange{4.0f}; // x will range from -4 to 4 but then changed by scrolling
  const float zoomSpeed{1.1f};

  float a{0.0f}, b{0.0f}, c{0.0f}, d{0.0f}, e{0.0f}, f{0.0f}, g{0.0f};
  int selectedCoefficient{0};
  float adjustmentSpeed{0.05f};

  char equationText[128] = {0};
  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_UP))
      xRange /= zoomSpeed; // zoom in
    if (IsKeyPressed(KEY_DOWN))
      xRange *= zoomSpeed; // zoom in
    if (xRange < 0.1f)
      xRange = 0.1f;
    if (xRange > 50.0f)
      xRange = 50.0f;

    float step{xRange * 2 / wavePoints}; // step size for plotting

    if (IsKeyPressed(KEY_RIGHT))
      selectedCoefficient = (selectedCoefficient + 1) % 7;
    if (IsKeyPressed(KEY_LEFT))
      selectedCoefficient = (selectedCoefficient + 6) % 7;

    if (GetMouseWheelMove() > 0) {
      if (selectedCoefficient == 0)
        a += adjustmentSpeed;
      else if (selectedCoefficient == 1)
        b += adjustmentSpeed;
      else if (selectedCoefficient == 2)
        c += adjustmentSpeed;
      else if (selectedCoefficient == 3)
        d += adjustmentSpeed;
      else if (selectedCoefficient == 4)
        e += adjustmentSpeed;
      else if (selectedCoefficient == 5)
        f += adjustmentSpeed;
      else if (selectedCoefficient == 6)
        g += adjustmentSpeed;
    }
    if (GetMouseWheelMove() < 0) {
      if (selectedCoefficient == 0)
        a -= adjustmentSpeed;
      else if (selectedCoefficient == 1)
        b -= adjustmentSpeed;
      else if (selectedCoefficient == 2)
        c -= adjustmentSpeed;
      else if (selectedCoefficient == 3)
        d -= adjustmentSpeed;
      else if (selectedCoefficient == 4)
        e -= adjustmentSpeed;
      else if (selectedCoefficient == 5)
        f -= adjustmentSpeed;
      else if (selectedCoefficient == 6)
        g -= adjustmentSpeed;
    }

    snprintf(equationText, sizeof(equationText),
             "y= %.2fx^6+%.2fx^5+%.2fx^4+%.2fx^3+%.2fx+%.2f", a, b, c, d, e, f,
             g);

    BeginDrawing();
    ClearBackground(BLACK);

    DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, GRAY);
    DrawLine(0, screenHeight / 2, screenWidth, screenHeight / 2, GRAY);

    DrawText("Y", screenWidth / 2 + 5, 5, 20, GRAY);
    DrawText("X", screenWidth - 20, screenHeight / 2 + 5, 20, GRAY);
    for (std::size_t i{0}; i < wavePoints - 1; ++i) {
      float x1{-xRange + i * step}; // current x value
      float x2{x1 + step};          // next x value
      float y1{polynomialWave(x1, a, b, c, d, e, f, g)};
      float y2{polynomialWave(x2, a, b, c, d, e, f, g)};

      // Map x and y values to screen coordinates
      Vector2 start = {screenWidth / 2 + x1 * (screenWidth / (2 * xRange)),
                       screenHeight / 2 - y1 * (screenHeight / (2 * xRange))};
      Vector2 end = {screenWidth / 2 + x2 * (screenWidth / (2 * xRange)),
                     screenHeight / 2 - y2 * (screenHeight / (2 * xRange))};
      DrawLineEx(start, end, 2.0f, WHITE);
    }
    DrawText(equationText, 10, 10, 20, YELLOW); // draw equation on text

    // Highlight the selected coefficient for clarity
    const char *coefficientNames[] = {"a", "b", "c", "d", "e", "f", "g"};
    char coefficientStatus[64];
    snprintf(coefficientStatus, sizeof(coefficientStatus),
             "Adjusting: %s (Scroll to change)",
             coefficientNames[selectedCoefficient]);
    DrawText(coefficientStatus, 10, 40, 20, BLUE);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
