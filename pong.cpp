#include <raylib.h>
class Ball {
public:
  float x, y;
  int speedX, speedY;
  int radius;

  void Draw() { DrawCircle(x, y, radius, WHITE); }
  void Update() {
    x += speedX;
    y += speedY;
    if (y + radius >= GetScreenHeight() || y - radius <= 0)
      speedY *= -1;
    if (x + radius >= GetScreenWidth() || x - radius <= 0)
      speedX *= -1;
  }
};

class Paddle {
public:
  float x, y;
  float width, height;
  int speed;

  void Draw() {

    DrawRectangle(
        x, y, width, height,
        WHITE); // Rectangle is drawn its topleft point just like the window
  };
  void Update() {
    if (IsKeyDown(KEY_UP))
      y -= speed;
    if (IsKeyDown(KEY_DOWN))
      y += speed;
    if (y <= 0)
      y = 0;
    if (y + height >= GetScreenHeight())
      y = GetScreenHeight() - height;
  }
};

Ball ball;
Paddle player;

int main() {
  Color green = {20, 160, 133, 255};
  // The paddles
  const int screenWidth = 1280;
  const int screenHeight = 800;
  const int rectangleHeight = 120;
  const int rectangleWidth = 25;
  // Ball attributes
  ball.x = screenWidth / 2;
  ball.y = screenHeight / 2;
  ball.speedX = 7;
  ball.speedY = 7;
  ball.radius = 20;

  // Player attributes
  player.width = 25;
  player.height = 120;
  player.x = player.width - 10;
  player.y = screenHeight / 2 - player.height / 2;
  player.speed = 6;
  InitWindow(screenWidth, screenHeight, "The pong");
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    BeginDrawing(); // Begin canvas drawing
    ClearBackground(BLACK);
    ball.Draw();
    ball.Update();
    player.Draw();
    player.Update();
    DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, WHITE);
    DrawRectangle(screenWidth - 35, (screenHeight - rectangleHeight) / 2,
                  rectangleWidth, rectangleHeight, WHITE);
    EndDrawing(); // End canvas drawing
  }

  CloseWindow();
  return 0;
}
