#include "SphereMesh.h"
#include "pch.h"
#include "raylib.h"
#include "raymath.h"

int main() {
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(1280, 720, "Planeta procedural — icoesfera");
  SetTargetFPS(60);

  Camera3D cam = {0};
  cam.position = Vector3{0.0f, 1.2f, 3.2f};
  cam.target = Vector3{0.0f, 0.0f, 0.0f};
  cam.up = Vector3{0.0f, 1.0f, 0.0f};
  cam.fovy = 45.0f;
  cam.projection = CAMERA_PERSPECTIVE;

  int subdiv = 3;
  SphereMesh sphere = SphereMesh(subdiv);
  Material mat = LoadMaterialDefault();
  bool wireframe = false;

  Shader sphereShader = LoadShader("assets/sphere.vs", "assets/sphere.fs");
  mat.shader = sphereShader;
  double lastModified = GetFileModTime("assets/sphere.fs");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    SetWindowTitle(TextFormat("GLSL shaders | fps: %d", GetFPS()));

    double currentModTime = GetFileModTime("assets/sphere.fs");
    if (currentModTime != lastModified) {
      UnloadShader(sphereShader);
      sphereShader = LoadShader(0, "assets/shader.fs");
      lastModified = currentModTime;
    }

    Vector3 lightPos = {3.0f, 5.0f, 3.0f};
    Vector3 viewPos = cam.position;
    SetShaderValue(sphereShader, GetShaderLocation(sphereShader, "lightPos"),
                   &lightPos, SHADER_UNIFORM_VEC3);
    SetShaderValue(sphereShader, GetShaderLocation(sphereShader, "viewPos"),
                   &viewPos, SHADER_UNIFORM_VEC3);

    BeginMode3D(cam);

    DrawMesh(sphere._mesh, mat, MatrixIdentity());

    EndMode3D();

    EndDrawing();
  }

  UnloadShader(sphereShader);
  CloseWindow();

  return 0;
}