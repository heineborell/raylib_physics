#include "raylib.h"
#include "raymath.h"
#include <cstdio>
#include <math.h>
#include <stdlib.h>
// Define the mathematical function z = f(x, y)
// You can modify this function to plot different surfaces
float surfaceFunction(float x, float y) {
  // Example: Sinusoidal wave surface
  // return 2.0f * sinf(x * 0.5f) * cosf(y * 0.5f);

  // Other examples you can try:
  // return x * x + y * y; // Paraboloid
  // return sinf(sqrtf(x*x + y*y)) / sqrtf(x*x + y*y + 0.1f); // Sinc function
  // return expf(-(x * x + y * y) * 0.1f) * cosf(x) * sinf(y); // Gaussian
  return 10.0f * (2.0f * x * y - 3.0f * x * x - 4.0f * y * y - 18.0f * x +
                  28.0f * y + 12.0f);
  // modulated
}

// Generate mesh vertices for the surface
Mesh GenerateSurfaceMesh(int resolutionX, int resolutionY, float rangeX,
                         float rangeY) {
  Mesh mesh = {0};

  // Calculate total vertices and triangles
  int vertexCount = resolutionX * resolutionY;
  int triangleCount = (resolutionX - 1) * (resolutionY - 1) * 2;

  // Allocate memory for mesh data
  mesh.vertexCount = vertexCount;
  mesh.triangleCount = triangleCount;
  mesh.vertices = (float *)malloc(vertexCount * 3 * sizeof(float));
  mesh.normals = (float *)malloc(vertexCount * 3 * sizeof(float));
  mesh.texcoords = (float *)malloc(vertexCount * 2 * sizeof(float));
  mesh.indices =
      (unsigned short *)malloc(triangleCount * 3 * sizeof(unsigned short));

  // Generate vertices
  float stepX = (2.0f * rangeX) / (resolutionX - 1);
  float stepY = (2.0f * rangeY) / (resolutionY - 1);

  for (int y = 0; y < resolutionY; y++) {
    for (int x = 0; x < resolutionX; x++) {
      int index = y * resolutionX + x;

      // Calculate world position
      float worldX = -rangeX + x * stepX;
      float worldY = -rangeY + y * stepY;
      float worldZ = surfaceFunction(worldX, worldY);

      // Set vertex position
      mesh.vertices[index * 3 + 0] = worldX;
      mesh.vertices[index * 3 + 1] = worldZ; // Y is up in raylib
      mesh.vertices[index * 3 + 2] = worldY;

      // Set texture coordinates
      mesh.texcoords[index * 2 + 0] = (float)x / (resolutionX - 1);
      mesh.texcoords[index * 2 + 1] = (float)y / (resolutionY - 1);
    }
  }

  // Calculate normals (simplified - using cross product of adjacent edges)
  for (int y = 0; y < resolutionY; y++) {
    for (int x = 0; x < resolutionX; x++) {
      int index = y * resolutionX + x;
      Vector3 normal = {0, 1, 0}; // Default up normal

      // Calculate normal using neighboring vertices (if available)
      if (x > 0 && x < resolutionX - 1 && y > 0 && y < resolutionY - 1) {
        Vector3 v1 = {mesh.vertices[(index + 1) * 3] -
                          mesh.vertices[(index - 1) * 3],
                      mesh.vertices[(index + 1) * 3 + 1] -
                          mesh.vertices[(index - 1) * 3 + 1],
                      mesh.vertices[(index + 1) * 3 + 2] -
                          mesh.vertices[(index - 1) * 3 + 2]};

        Vector3 v2 = {mesh.vertices[(index + resolutionX) * 3] -
                          mesh.vertices[(index - resolutionX) * 3],
                      mesh.vertices[(index + resolutionX) * 3 + 1] -
                          mesh.vertices[(index - resolutionX) * 3 + 1],
                      mesh.vertices[(index + resolutionX) * 3 + 2] -
                          mesh.vertices[(index - resolutionX) * 3 + 2]};

        normal = Vector3CrossProduct(v1, v2);
        normal = Vector3Normalize(normal);
      }

      mesh.normals[index * 3 + 0] = normal.x;
      mesh.normals[index * 3 + 1] = normal.y;
      mesh.normals[index * 3 + 2] = normal.z;
    }
  }

  // Generate indices for triangles
  int triangleIndex = 0;
  for (int y = 0; y < resolutionY - 1; y++) {
    for (int x = 0; x < resolutionX - 1; x++) {
      int topLeft = y * resolutionX + x;
      int topRight = topLeft + 1;
      int bottomLeft = (y + 1) * resolutionX + x;
      int bottomRight = bottomLeft + 1;

      // First triangle
      mesh.indices[triangleIndex * 3 + 0] = topLeft;
      mesh.indices[triangleIndex * 3 + 1] = bottomLeft;
      mesh.indices[triangleIndex * 3 + 2] = topRight;
      triangleIndex++;

      // Second triangle
      mesh.indices[triangleIndex * 3 + 0] = topRight;
      mesh.indices[triangleIndex * 3 + 1] = bottomLeft;
      mesh.indices[triangleIndex * 3 + 2] = bottomRight;
      triangleIndex++;
    }
  }

  // Upload mesh to GPU
  UploadMesh(&mesh, false);

  return mesh;
}

int main(void) {
  // Window dimensions
  const int screenWidth = 1200;
  const int screenHeight = 800;

  InitWindow(screenWidth, screenHeight, "3D Surface Plot - Raylib");

  // Define camera
  Camera3D camera = {0};
  camera.position = (Vector3){15.0f, 15.0f, 15.0f};
  camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  // Generate surface mesh
  int resolution = 100; // Grid resolution
  float range = 5.0f;   // Range of x and y values
  Mesh surfaceMesh = GenerateSurfaceMesh(resolution, resolution, range, range);

  // Create material
  Material material = LoadMaterialDefault();
  material.maps[MATERIAL_MAP_DIFFUSE].color = BLUE;

  // Create model from mesh
  Model surfaceModel = LoadModelFromMesh(surfaceMesh);
  surfaceModel.materials[0] = material;

  // Wireframe toggle
  bool wireframe = false;

  SetTargetFPS(60);

  // Main game loop
  while (!WindowShouldClose()) {
    // Update camera
    UpdateCamera(&camera, CAMERA_ORBITAL);

    // Toggle wireframe mode
    if (IsKeyPressed(KEY_SPACE)) {
      wireframe = !wireframe;
    }

    // Reset camera position
    if (IsKeyPressed(KEY_R)) {
      camera.position = (Vector3){15.0f, 15.0f, 15.0f};
      camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    }

    // Drawing
    BeginDrawing();

    ClearBackground(BLACK);

    BeginMode3D(camera);

    // Draw coordinate axes
    DrawLine3D((Vector3){-range, 0, 0}, (Vector3){range, 0, 0}, RED);  // X axis
    DrawLine3D((Vector3){0, -5, 0}, (Vector3){0, 5, 0}, GREEN);        // Y axis
    DrawLine3D((Vector3){0, 0, -range}, (Vector3){0, 0, range}, BLUE); // Z axis

    // Draw surface
    if (wireframe) {
      DrawModelWires(surfaceModel, (Vector3){0, 0, 0}, 1.0f, WHITE);
    } else {
      DrawModel(surfaceModel, (Vector3){0, 0, 0}, 1.0f,
                material.maps[MATERIAL_MAP_DIFFUSE].color);
    }

    // Draw grid on XZ plane
    DrawGrid(20, 1.0f);

    EndMode3D();

    // UI
    DrawText("3D Surface Plot", 10, 10, 20, WHITE);
    DrawText("Controls:", 10, 40, 16, WHITE);
    DrawText("- Mouse: Rotate camera", 10, 60, 14, GRAY);
    DrawText("- Mouse wheel: Zoom", 10, 80, 14, GRAY);
    DrawText("- SPACE: Toggle wireframe", 10, 100, 14, GRAY);
    DrawText("- R: Reset camera", 10, 120, 14, GRAY);
    DrawText("- ESC: Exit", 10, 140, 14, GRAY);

    char modeText[50];
    sprintf(modeText, "Mode: %s", wireframe ? "Wireframe" : "Solid");
    DrawText(modeText, 10, 170, 14, wireframe ? YELLOW : BLUE);

    DrawFPS(screenWidth - 100, 10);

    EndDrawing();
  }

  // Cleanup
  UnloadModel(surfaceModel);
  CloseWindow();

  return 0;
}
