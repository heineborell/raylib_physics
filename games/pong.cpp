#include <raylib.h>

// Globals for scores
int player_score{0};
int cpu_score{0};

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
    if (x + radius >= GetScreenWidth())
      player_score++;
    if (x + radius >= GetScreenWidth() || x - radius <= 0)
      speedX *= -1;
    if (x - radius < 0)
      cpu_score++;
  }
};

class Paddle {
public:
  float x, y;
  float width, height;
  int speed;

  void LimitMovement() {
    if (y <= 0)
      y = 0;
    if (y + height >= GetScreenHeight())
      y = GetScreenHeight() - height;
  }
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
    LimitMovement();
  }
};

class CpuPaddle : public Paddle {
public:
  // for cpu to follow the ball
  void Update(int ball_y) {
    if (y + height / 2 > ball_y)
      y = y - speed;
    if (y + height / 2 <= ball_y)
      y = y + speed;
    LimitMovement();
  };
}; // Inheriting from public paddle

Ball ball;
Paddle player;
CpuPaddle cpu;

int main() {
  // screen dimensions
  const int screenWidth{1280};
  const int screenHeight{800};
  // Paddle dimensions actually we define them below to so not much necessary
  const int rectangleHeight{120};
  const int rectangleWidth{25};
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
  // cpu attributes
  cpu.width = 25;
  cpu.height = 120;
  cpu.x = screenWidth - cpu.width - 10;
  cpu.y = screenHeight / 2 - cpu.height / 2;
  cpu.speed = 5;

  InitWindow(screenWidth, screenHeight, "The pong");
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    BeginDrawing(); // Begin canvas drawing
    ClearBackground(BLACK);

    // Update positions
    ball.Update();
    player.Update();
    cpu.Update(ball.y);

    // check for collisions
    if (CheckCollisionCircleRec(
            Vector2{ball.x, ball.y}, ball.radius,
            Rectangle{player.x, player.y, player.width, player.height}))
      ball.speedX *= -1;
    ;

    if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius,
                                Rectangle{cpu.x, cpu.y, cpu.width, cpu.height}))
      ball.speedX *= -1;
    ;

    // Draw stuff
    DrawLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, WHITE);
    ball.Draw();
    player.Draw();
    cpu.Draw();
    DrawText(TextFormat("%i", player_score), screenWidth / 4 - 20, 20, 80,
             WHITE);
    DrawText(TextFormat("%i", cpu_score), 3 * screenWidth / 4 - 20, 20, 80,
             WHITE);
    EndDrawing(); // End canvas drawing
  }

  CloseWindow();
  return 0;
}
