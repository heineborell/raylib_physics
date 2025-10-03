#include <cstddef>
#include <raylib.h>
#include <vector>

// clang-format off
const int level[] = {0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 1, 0,
                     0, 0, 0, 0, 0, 1, 0, 0, 
                     0, 0, 0, 1, 0, 0, 0, 0};
// clang-format on
enum SpriteDirection { Left = -1, Right = 1 };

struct Sprite {
  Texture2D texture;
  Rectangle destRect; // width and height of the texture!
  Vector2 velocity;
  SpriteDirection dir;
};

void movePlayer(Sprite &player) {
  player.velocity.x = 0;
  if (IsKeyDown(KEY_D)) {
    player.velocity.x = 100.0;
    player.dir = SpriteDirection::Right;
  }
  if (IsKeyDown(KEY_A)) {
    player.velocity.x = -100.0;
    player.dir = SpriteDirection::Left;
  }
  if (IsKeyPressed(KEY_SPACE)) {
    player.velocity.y = -1000;
  }
}

void applyGravity(Sprite &sprite) {
  sprite.velocity.y += 100.0;
  if (sprite.velocity.y > 600.0) {
    sprite.velocity.y = 600;
  }
}

void applyVelocity_x(Sprite &sprite) {
  sprite.destRect.x += sprite.velocity.x * GetFrameTime();
}

void applyVelocity_y(Sprite &sprite) {
  sprite.destRect.y += sprite.velocity.y * GetFrameTime();
}

void checkCollisions_y(Sprite &sprite, std::vector<Sprite> &tiles) {
  for (const auto &tile : tiles) {
    // if player rect untersect tiles rect
    if (CheckCollisionRecs(sprite.destRect, tile.destRect)) {
      // reverse the overlap
      // sprite on bottom
      if (sprite.destRect.y > tile.destRect.y)
        sprite.destRect.y = tile.destRect.y + tile.destRect.height;
      else
        sprite.destRect.y = tile.destRect.y - tile.destRect.height;
    }
  }
}

void checkCollisions_x(Sprite &sprite, std::vector<Sprite> &tiles) {
  for (const auto &tile : tiles) {
    // if player rect untersect tiles rect
    if (CheckCollisionRecs(sprite.destRect, tile.destRect)) {
      // reverse the overlap
      // sprite on bottom
      if (sprite.destRect.x > tile.destRect.x)
        sprite.destRect.x = tile.destRect.x + tile.destRect.width;
      else
        sprite.destRect.x = tile.destRect.x - tile.destRect.width;
    }
  }
}
std::vector<Sprite> loadLevel(Texture2D tempTexture) {
  std::vector<Sprite> sprites;
  const int levelWidth = 8;
  const int levelHeight = 5;
  for (size_t i{0}; i < levelHeight * levelWidth; ++i) {
    size_t x{i % levelWidth};
    size_t y{i / levelWidth};
    // if number is >0 then its a sprite add to list
    if (level[i] > 0) {
      sprites.push_back(Sprite{.texture = tempTexture,
                               .destRect = Rectangle{.x = x * 32.0F,
                                                     .y = y * 32.0F,
                                                     .width = 32.0F,
                                                     .height = 32.0F}});
    }
  }
  return sprites;
}

int main() {
  Color GREY{29, 29, 29, 255};
  const int screenWidth{600};
  const int screenHeight{400};
  const int FPS{60};

  InitWindow(screenWidth, screenHeight, "awesome game");

  Texture2D playerIdle{
      LoadTexture("../sphere/assets/herochar "
                  "sprites(new)/herochar_idle_anim_strip_4.png")};
  SetTargetFPS(FPS);
  Sprite player{playerIdle, {10, 32, 32, 32}};

  std::vector<Sprite> levelTiles{loadLevel(playerIdle)};

  // Simulation Loop
  while (!WindowShouldClose()) {
    // Update Section
    movePlayer(player);
    applyGravity(player);

    // after movement update
    applyVelocity_y(player);
    checkCollisions_y(player, levelTiles);
    applyVelocity_x(player);
    checkCollisions_x(player, levelTiles);

    // to stay on ground
    if (player.destRect.y > GetScreenHeight() - player.destRect.height) {
      player.destRect.y = GetScreenHeight() - player.destRect.height;
    }

    // Draw Section
    BeginDrawing();
    ClearBackground(
        SKYBLUE); // clearing the background to grey before loop is run again
    // draw the level tiles
    for (const auto &tile : levelTiles) {
      DrawTexturePro(tile.texture, {0, 0, 16, 16}, tile.destRect, {0, 0}, 0.0,
                     RAYWHITE);
    }

    DrawTexturePro(player.texture, {0, 0, 16, 16}, player.destRect, {0, 0}, 0.0,
                   RAYWHITE);

    EndDrawing();
  }

  UnloadTexture(playerIdle);

  CloseWindow();
  return 0;
}
