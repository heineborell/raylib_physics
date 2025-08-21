#include <raylib.h>

int main() {
  int ballX = 400;
  int ballY = 400;
  Color green = {20, 160, 133, 255};
  InitWindow(800, 800, "The Window");
  SetTargetFPS(60);
  while (!WindowShouldClose()) {

    if (IsKeyPressed(KEY_DOWN))
      ballY += 3;
    else if (IsKeyPressed(KEY_UP))
      ballY -= 3;
    if (IsKeyPressed(KEY_RIGHT))
      ballX += 3;
    else if (IsKeyPressed(KEY_LEFT))
      ballX -= 3;

    BeginDrawing();
    ClearBackground(green);
    DrawCircle(ballX, ballY, 50, WHITE);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
