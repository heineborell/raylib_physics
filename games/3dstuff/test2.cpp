#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdlib.h>

// Function to generate a custom mesh from a mathematical function
// f(u, v) returns a Vector3 position for parameters u and v
Mesh GenMeshFromFunction(Vector3 (*func)(float, float), float uMin, float uMax,
                         int uSteps, float vMin, float vMax, int vSteps) {
  Mesh mesh = {0};

  // Calculate vertex count
  int vertexCount = (uSteps + 1) * (vSteps + 1);
  int triangleCount = uSteps * vSteps * 2;

  // Allocate memory
  mesh.vertexCount = vertexCount;
  mesh.triangleCount = triangleCount;

  mesh.vertices = (float *)RL_MALLOC(vertexCount * 3 * sizeof(float));
  mesh.texcoords = (float *)RL_MALLOC(vertexCount * 2 * sizeof(float));
  mesh.normals = (float *)RL_MALLOC(vertexCount * 3 * sizeof(float));
  mesh.indices =
      (unsigned short *)RL_MALLOC(triangleCount * 3 * sizeof(unsigned short));

  // Generate vertices
  int vertexIndex = 0;
  for (int i = 0; i <= uSteps; i++) {
    float u = uMin + (uMax - uMin) * i / (float)uSteps;

    for (int j = 0; j <= vSteps; j++) {
      float v = vMin + (vMax - vMin) * j / (float)vSteps;

      // Get position from function
      Vector3 pos = func(u, v);

      mesh.vertices[vertexIndex * 3 + 0] = pos.x;
      mesh.vertices[vertexIndex * 3 + 1] = pos.y;
      mesh.vertices[vertexIndex * 3 + 2] = pos.z;

      // Generate UV coordinates
      mesh.texcoords[vertexIndex * 2 + 0] = (float)i / uSteps;
      mesh.texcoords[vertexIndex * 2 + 1] = (float)j / vSteps;

      vertexIndex++;
    }
  }

  // Generate indices for triangles
  int triIndex = 0;
  for (int i = 0; i < uSteps; i++) {
    for (int j = 0; j < vSteps; j++) {
      int v0 = i * (vSteps + 1) + j;
      int v1 = v0 + 1;
      int v2 = v0 + (vSteps + 1);
      int v3 = v2 + 1;

      // First triangle
      mesh.indices[triIndex++] = v0;
      mesh.indices[triIndex++] = v2;
      mesh.indices[triIndex++] = v1;

      // Second triangle
      mesh.indices[triIndex++] = v1;
      mesh.indices[triIndex++] = v2;
      mesh.indices[triIndex++] = v3;
    }
  }

  // Calculate normals manually
  // Initialize normals to zero
  for (int i = 0; i < vertexCount * 3; i++) {
    mesh.normals[i] = 0.0f;
  }

  // Calculate normals by averaging face normals
  for (int i = 0; i < triangleCount; i++) {
    int idx0 = mesh.indices[i * 3 + 0];
    int idx1 = mesh.indices[i * 3 + 1];
    int idx2 = mesh.indices[i * 3 + 2];

    Vector3 v0 = {mesh.vertices[idx0 * 3], mesh.vertices[idx0 * 3 + 1],
                  mesh.vertices[idx0 * 3 + 2]};
    Vector3 v1 = {mesh.vertices[idx1 * 3], mesh.vertices[idx1 * 3 + 1],
                  mesh.vertices[idx1 * 3 + 2]};
    Vector3 v2 = {mesh.vertices[idx2 * 3], mesh.vertices[idx2 * 3 + 1],
                  mesh.vertices[idx2 * 3 + 2]};

    Vector3 edge1 = Vector3Subtract(v1, v0);
    Vector3 edge2 = Vector3Subtract(v2, v0);
    Vector3 normal = Vector3CrossProduct(edge1, edge2);

    // Add to each vertex normal
    mesh.normals[idx0 * 3 + 0] += normal.x;
    mesh.normals[idx0 * 3 + 1] += normal.y;
    mesh.normals[idx0 * 3 + 2] += normal.z;

    mesh.normals[idx1 * 3 + 0] += normal.x;
    mesh.normals[idx1 * 3 + 1] += normal.y;
    mesh.normals[idx1 * 3 + 2] += normal.z;

    mesh.normals[idx2 * 3 + 0] += normal.x;
    mesh.normals[idx2 * 3 + 1] += normal.y;
    mesh.normals[idx2 * 3 + 2] += normal.z;
  }

  // Normalize all normals
  for (int i = 0; i < vertexCount; i++) {
    Vector3 n = {mesh.normals[i * 3], mesh.normals[i * 3 + 1],
                 mesh.normals[i * 3 + 2]};
    n = Vector3Normalize(n);
    mesh.normals[i * 3 + 0] = n.x;
    mesh.normals[i * 3 + 1] = n.y;
    mesh.normals[i * 3 + 2] = n.z;
  }

  // Upload mesh to GPU
  UploadMesh(&mesh, false);

  return mesh;
}

// Example mathematical functions
Vector3 WaveFunction(float u, float v) {
  return (Vector3){u, sinf(u) * cosf(v), v};
}

Vector3 SphereFunction(float u, float v) {
  float radius = 2.0f;
  return (Vector3){radius * sinf(u) * cosf(v), radius * sinf(u) * sinf(v),
                   radius * cosf(u)};
}

Vector3 TorusFunction(float u, float v) {
  float R = 2.0f; // Major radius
  float r = 0.5f; // Minor radius
  return (Vector3){(R + r * cosf(v)) * cosf(u), r * sinf(v),
                   (R + r * cosf(v)) * sinf(u)};
}

Vector3 ParaboloidFunction(float u, float v) {
  return (Vector3){u, u * u + v * v, v};
}

int main(void) {
  const int screenWidth = 1200;
  const int screenHeight = 800;

  InitWindow(screenWidth, screenHeight, "Raylib - Custom Mathematical Mesh");

  Camera3D camera = {0};
  camera.position = (Vector3){8.0f, 8.0f, 8.0f};
  camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  // Generate mesh from mathematical function
  Mesh customMesh =
      GenMeshFromFunction(WaveFunction, -5.0f, 5.0f, 50, -5.0f, 5.0f, 50);
  Model model = LoadModelFromMesh(customMesh);

  // Create material
  model.materials[0].shader = LoadShader(0, 0);

  int currentFunction = 0;
  const char *functionNames[] = {"Wave", "Sphere", "Torus", "Paraboloid"};

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    UpdateCamera(&camera, CAMERA_ORBITAL);

    // Switch between different functions
    if (IsKeyPressed(KEY_SPACE)) {
      UnloadModel(model);
      currentFunction = (currentFunction + 1) % 4;

      switch (currentFunction) {
      case 0:
        customMesh =
            GenMeshFromFunction(WaveFunction, -5.0f, 5.0f, 50, -5.0f, 5.0f, 50);
        break;
      case 1:
        customMesh =
            GenMeshFromFunction(SphereFunction, 0, PI, 40, 0, 2 * PI, 40);
        break;
      case 2:
        customMesh =
            GenMeshFromFunction(TorusFunction, 0, 2 * PI, 40, 0, 2 * PI, 20);
        break;
      case 3:
        customMesh = GenMeshFromFunction(ParaboloidFunction, -3.0f, 3.0f, 40,
                                         -3.0f, 3.0f, 40);
        break;
      }

      model = LoadModelFromMesh(customMesh);
      model.materials[0].shader = LoadShader(0, 0);
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);
    DrawModel(model, (Vector3){0, 0, 0}, 1.0f, BLUE);
    DrawGrid(10, 1.0f);
    EndMode3D();

    DrawText(TextFormat("Function: %s", functionNames[currentFunction]), 10, 10,
             20, DARKGRAY);
    DrawText("Press SPACE to change function", 10, 40, 20, DARKGRAY);
    DrawText("Mouse to rotate camera", 10, 70, 20, DARKGRAY);

    EndDrawing();
  }

  UnloadModel(model);
  CloseWindow();

  return 0;
}
