#include "raylib.h"
#include <cmath>

// ---------------------------------------------------------
// Example function: z = sin(x) * cos(y)
// Replace with any function you want
// ---------------------------------------------------------
float f(float x, float y) { return std::sinf(x) * std::cosf(y); }

// ---------------------------------------------------------
// Generate a mesh from a mathematical function f(x,y)
// width, height: number of samples
// spacing: distance between grid points
// ---------------------------------------------------------
Mesh GenerateFunctionMesh(int width, int height, float spacing) {
  Mesh mesh = {0};

  int vertexCount = width * height;
  int triangleCount = (width - 1) * (height - 1) * 2;

  mesh.vertexCount = vertexCount;
  mesh.triangleCount = triangleCount;

  mesh.vertices = (float *)MemAlloc(vertexCount * 3 * sizeof(float));
  mesh.texcoords = (float *)MemAlloc(vertexCount * 2 * sizeof(float));
  mesh.normals = (float *)MemAlloc(vertexCount * 3 * sizeof(float));
  mesh.indices =
      (unsigned short *)MemAlloc(triangleCount * 3 * sizeof(unsigned short));

  // ---------- Fill vertex positions ----------
  int k = 0;
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {

      float x = (i - width / 2) * spacing;
      float y = (j - height / 2) * spacing;
      float z = f(x, y);

      mesh.vertices[k * 3 + 0] = x;
      mesh.vertices[k * 3 + 1] = z;
      mesh.vertices[k * 3 + 2] = y;

      mesh.texcoords[k * 2 + 0] = (float)i / (width - 1);
      mesh.texcoords[k * 2 + 1] = (float)j / (height - 1);

      k++;
    }
  }

  // ---------- Fill triangle indices ----------
  int ti = 0;
  for (int j = 0; j < height - 1; j++) {
    for (int i = 0; i < width - 1; i++) {

      int idx = j * width + i;

      // Triangle 1
      mesh.indices[ti++] = idx;
      mesh.indices[ti++] = idx + 1;
      mesh.indices[ti++] = idx + width;

      // Triangle 2
      mesh.indices[ti++] = idx + 1;
      mesh.indices[ti++] = idx + width + 1;
      mesh.indices[ti++] = idx + width;
    }
  }

  UploadMesh(&mesh, true);
  return mesh;
}

// ---------------------------------------------------------
// Main
// ---------------------------------------------------------
int main() {
  InitWindow(1200, 800, "Function Mesh (C++ version)");

  Mesh mesh = GenerateFunctionMesh(200, 200, 0.05f);
  Model model = LoadModelFromMesh(mesh);

  Camera3D cam = {0};
  cam.position = {3.0f, 3.0f, 3.0f};
  cam.target = {0.0f, 0.0f, 0.0f};
  cam.up = {0.0f, 1.0f, 0.0f};
  cam.fovy = 45.0f;
  cam.projection = CAMERA_PERSPECTIVE;

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    UpdateCamera(&cam, CAMERA_FREE);

    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(cam);
    DrawModel(model, {0, 0, 0}, 1.0f, WHITE);
    DrawGrid(20, 0.5f);
    EndMode3D();

    DrawFPS(20, 20);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
