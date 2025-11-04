#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <list> //c++ stl list efficient for frequant insertions deletions
#include <vector>

struct Projectile {
  Vector3 position;
  Vector3 velocity;
  float lifetime;
  float radius;
  Color color;
  bool active; // check if its active or need to be removed
};

int main() {
  const int screenWidth{1200};
  const int screenHeight{720};

  InitWindow(screenWidth, screenHeight, "3D stuff");
  ;

  DisableCursor(); // this is for mouse style look there are other. HideCursor()
                   // hides doesnt lock, ShowCursor() shows it back

  Camera3D camera{0}; // initlaize all members to default
  // camera.target = Vector3 {0.0f,0.0f,0.0f};
  // camera.up = Vector3{0.0f, 1.0f, 0.0f}; // direction up for camera, can be
  // set in game loop
  camera.fovy = 60.0f; // field of view on the Y axis in degress common 45-90
                       // larger fish eye
  // camera.projection: type of projection
  // CAMERA_PERSPECTIVE realistic 3d view
  // CAMERA_ORTHOGRAPHIC
  camera.projection = CAMERA_PERSPECTIVE;

  // Terrain generation
  const float terrainWorldWidth{500.0f};
  const float terrainWorldDepth{500.0f};
  const float terrainMaxHeight{20.f};
  const int terrainImgWidth{200};
  const int terrainImgHeight{200};

  // generate a heightmap image
  Image heightmapImage =
      GenImageCellular(terrainImgWidth, terrainImgHeight, 16);
  // Image heightmapImage =
  //     GenImagePerlinNoise(terrrainImgWidth, terrrainImgHeight, 0, 0, 20.0f);
  // Create a 3D mesh
  Mesh terrainMesh = GenMeshHeightmap(
      heightmapImage,
      {terrainWorldWidth, terrainMaxHeight,
       terrainWorldDepth}); // vector feined dimensions of the terrain model

  UnloadImage(heightmapImage); // unload as you dont need
  //
  // Load generated img into model structure which hold multiple meshes and
  // materials making esaier to draw
  Model terrainModel{LoadModelFromMesh(terrainMesh)};

  // set terrain apperance. models have materials and materials have maps
  terrainModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = GRAY;
  // Texture2D terraintex = LoadTexture(const char *fileName)
  // terrainModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = GRAY;

  // Spaceship setup
  Mesh cubeMesh = GenMeshCube(2.0f, 2.0f, 2.0f);
  // Model shipModel = LoadModel()
  Model shipModel = LoadModelFromMesh(cubeMesh);
  shipModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = MAROON;

  // Ship position
  Vector3 shipPosition = {terrainWorldWidth / 2.0f, terrainImgHeight / 2.0f,
                          terrainWorldDepth / 2.0f};
  float shipYaw = 0.0f;   // local y axis turn left/right
  float shipPitch = 0.0f; // local x axis up/down, nor roll for z axis
  //
  const float mouseSensitivity =
      0.003f; // how mch rootation per pixel movement of the mouse
  const float moveSpeed = 25.0f;

  // Projectiles
  std::list<Projectile> projectiles; // better to remove add in the midlle
  const int MAX_PROJECTILES = 50;
  const float PROJECTILE_SPEED = 70.0f;
  const float PROJECTILE_LIFETIME = 3.0f;
  const float PROJECTILE_RADIUS = 0.2f;
  Color PROJECTILE_COLOR = ORANGE;

  float shootCooldown = 0.0f;
  const float SHOOT_RATE = 0.15f;
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    float dt = GetFrameTime(); // get the frametime elapsed since the last frame

    // Ship Control
    Vector2 mouseDelta =
        GetMouseDelta(); // get how much mouse moved since the last frame
    shipYaw -= mouseDelta.x * mouseSensitivity;
    shipPitch -= mouseDelta.y * mouseSensitivity;

    // clamp pitch
    shipPitch = Clamp(shipPitch, -PI / 2.0f + 0.01f, PI / 2.0f - 0.01f);

    // ship orientation with rotation matrix
    // 1. Rotate around Y-axis for yaw
    // 2. Rotate around new xaxis for pitch
    Matrix shipRotationMatrix =
        MatrixMultiply(MatrixRotateX(shipPitch), MatrixRotateY(shipYaw));
    // if roll included just at matrixrotatez too
    //  SHips update position
    //  going forward is positive movement in the z axis (0,0,1)
    Vector3 shipForward = Vector3Transform({0, 0, 1}, shipRotationMatrix);
    Vector3 shipRight = Vector3Transform({1, 0, 0}, shipRotationMatrix);
    Vector3 shipUp = Vector3Transform({0, 1, 0}, shipRotationMatrix);
    // Vector3 shipUp = Vector3Normalize(Vector3CrossProduct(shipRight,
    // shipForward));// this is more robust ?

    // Ship control basically position
    Vector3 moveDirection = {0}; // Start with no movement
    if (IsKeyDown(KEY_W))
      moveDirection = Vector3Add(moveDirection, shipForward);
    if (IsKeyDown(KEY_S))
      moveDirection = Vector3Subtract(moveDirection, shipForward);
    if (IsKeyDown(KEY_A))
      moveDirection = Vector3Subtract(moveDirection, shipRight);
    if (IsKeyDown(KEY_D))
      moveDirection = Vector3Add(moveDirection, shipRight);

    if (Vector3LengthSqr(moveDirection) > 0.001f) {
      moveDirection = Vector3Normalize(moveDirection);
      shipPosition =
          Vector3Add(shipPosition, Vector3Scale(moveDirection, moveSpeed * dt));
    }

    // I skip the projectile for now
    //  camera update
    float cameraDistance = 15.0f; // how fara behind the camera is
    float cameraHeightOffset =
        5.0f; // how high above the ships centerline camera is

    // calculate desired offset
    Vector3 cameraOffset = Vector3Scale(
        shipForward,
        -cameraDistance); // go backward from the ships forward vector
    // add some height
    cameraOffset.y += cameraHeightOffset;
    camera.position = Vector3Add(shipPosition, cameraOffset);
    camera.target = shipPosition;
    camera.up = {0, 1, 0}; // use global (0,1,0) to set up cameras up direction

    BeginDrawing();
    ClearBackground(SKYBLUE);

    // Enter 3D mode all drawing 3d will be rendered using specified camera
    BeginMode3D(camera);
    DrawModelWires(terrainModel, Vector3Zero(), 1.0f, LIME);
    DrawModel(terrainModel, Vector3Zero(), 1.0f, WHITE);

    // Draw Ship
    //  1-create rotation matrix (shipRotationMatrix)
    //  2-create translation matrix
    //  3-multiply theese to get final transformation matrix
    //  order for model transform scale-rotate-translate
    //  not scaling here
    shipModel.transform = MatrixMultiply(
        shipRotationMatrix,
        MatrixTranslate(shipPosition.x, shipPosition.y, shipPosition.z));

    // we draw it at (0,0,0) then transform handles it
    DrawModel(shipModel, Vector3Zero(), 1.0f, WHITE);

    // Helper grid
    DrawGrid(100, 10.0f);
    EndMode3D();
    DrawText("demo", 10, 10, 20, DARKGRAY);
    DrawText(TextFormat("yaw:%.2f Pitch: %.2f", shipYaw * RAD2DEG,
                        shipPitch * RAD2DEG),
             10, 80, 10, GRAY);
    DrawFPS(screenWidth - 90, 10);
    EndDrawing();
  }
  UnloadModel(terrainModel);
  UnloadModel(shipModel);

  CloseWindow();

  return 0;
}
