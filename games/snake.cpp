#include <deque>
#include <iostream>
#include <raylib.h>
#include <raymath.h>

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

// Size of each of grid boxes
int cellSize = 30;
int cellCount = 25;

// this is to make the snake move slower
double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, std::deque<Vector2> deque) {
  for (int i = 0; i < deque.size(); i++) {
    if (Vector2Equals(deque[i], element))
      return true;
  };
  return false;
}

bool EventTriggered(double interval) {
  double currentTime = GetTime();
  if (currentTime - lastUpdateTime >= interval) {
    lastUpdateTime = currentTime;
    return true;
  };
  return false;
}

class Snake {
public:
  std::deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9},
                              Vector2{4, 9}}; // this deque is cool object
  Vector2 direction = {1, 0};

  bool addSegment = false;
  void Draw() {
    for (int i = 0; i < body.size(); i++) {
      float x = body[i].x;
      float y = body[i].y;
      Rectangle segment =
          Rectangle{x * cellSize, y * cellSize, static_cast<float>(cellSize),
                    static_cast<float>(cellSize)};
      DrawRectangleRounded(segment, 0.5, 6, darkGreen);
    };
  }

  void Update() {
    body.push_front(Vector2Add(body[0], direction));
    if (addSegment) {
      addSegment = false;
    } else {
      body.pop_back(); // pop element back
    } // need to add vectors so gotta include raymath.h
  }

}; // This dude will use a deque which makes possible to put
   // withdraw elements from both ends of the array

class Food {
public:
  Vector2 position;  // vector is a struct that hold two values
  Texture2D texture; // texture rendering

  void Draw() {
    DrawTexture(texture, position.x * cellSize, position.y * cellSize, WHITE);
  }

  Food(std::deque<Vector2> snakeBody) {
    // Return and image pixel data of a graphical image
    Image image = LoadImage("food.png");
    texture = LoadTextureFromImage(
        image); // returns texture2D struct, optimised data type for graphics
    UnloadImage(image); // free memory and unload image
    position = GenerateRandomPos(snakeBody);
  }
  // Now we have to kill the constructed object not to create bunch of stuff
  // around releasing source
  ~Food() { UnloadTexture(texture); }

  Vector2 GenerateRandomCell() {
    float x = GetRandomValue(0, cellCount - 1);
    float y = GetRandomValue(0, cellCount - 1);
    return Vector2{x, y};
  }

  Vector2 GenerateRandomPos(std::deque<Vector2> snakeBody) {
    Vector2 position = GenerateRandomCell();
    while (ElementInDeque(position, snakeBody)) {
      position = GenerateRandomCell();
    }
    return position;
  }
};

class Game {
public:
  Snake snake = Snake();
  Food food = Food(snake.body);
  void Draw() {
    food.Draw();
    snake.Draw();
  }

  void Update() {
    snake.Update();
    CheckCollisionWithFood();
  }

  void CheckCollisionWithFood() {
    if (Vector2Equals(snake.body[0], food.position)) {
      food.position = food.GenerateRandomPos(snake.body);
      snake.addSegment = true;
    }
  }
};

int main() {
  // screen dimensions
  InitWindow(cellCount * cellSize, cellCount * cellSize, "Snake");
  SetTargetFPS(60);
  Game game = Game();

  while (!WindowShouldClose()) {
    BeginDrawing();
    if (EventTriggered(0.2)) {
      game.Update();
    } // gotta update this not every 60 secs so that snake is not too fast

    // Key presses
    if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) {
      game.snake.direction = {0, -1};
    } // remember origin is at top left
    //
    if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
      game.snake.direction = {0, 1};
    } // remember origin is at top left
    if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
      game.snake.direction = {-1, 0};
    } // remember origin is at top left
    if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
      game.snake.direction = {1, 0};
    } // remember origin is at top left

    ClearBackground(green);
    game.Draw();
    EndDrawing();
  }
  CloseWindow();
  return 0;
}
