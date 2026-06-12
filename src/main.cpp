#include "CameraController.h"
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

  int subdiv = 6;
  SphereMesh sphere = SphereMesh(subdiv);
  Material mat = LoadMaterialDefault();
  bool wireframe = false;

  const char *sphereShaderFileFS = "assets/sphere.fs";
  const char *sphereShaderFileVS = "assets/sphere.vs";
  Shader sphereShader = LoadShader(sphereShaderFileVS, sphereShaderFileFS);
  mat.shader = sphereShader;
  double lastModified = GetFileModTime(sphereShaderFileFS);

  CameraController camController;

  Material matWire = LoadMaterialDefault(); // shader default, sin luz
  matWire.maps[MATERIAL_MAP_DIFFUSE].color = BLACK;

  char *code = LoadFileText("assets/double.compute");
  unsigned int csId = rlLoadShader(code, RL_COMPUTE_SHADER);
  unsigned int program = rlLoadShaderProgramCompute(csId);
  UnloadFileText(code);

  std::vector<float> data(1000, 1.5f);
  unsigned int ssbo = rlLoadShaderBuffer(data.size() * sizeof(float),
                                         data.data(), RL_DYNAMIC_COPY);

  rlEnableShader(program);
  rlBindShaderBuffer(ssbo, 0); // binding = 0, igual que en el GLSL
  rlComputeShaderDispatch((1000 + 255) / 256, 1,
                          1); // ceil(N / local_size_x) workgroups
  rlDisableShader();

  rlReadShaderBuffer(ssbo, data.data(), data.size() * sizeof(float), 0);
  TraceLog(LOG_INFO, "data[0] = %f, data[999] = %f", data[0],
           data[999]); // 3.0 y 3.0

  rlUnloadShaderBuffer(ssbo);
  rlUnloadShaderProgram(program);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground({51, 51, 51, 255});
    SetWindowTitle(TextFormat("GLSL shaders | fps: %d", GetFPS()));

    double currentModTime = GetFileModTime(sphereShaderFileFS);
    if (currentModTime != lastModified) {
      UnloadShader(sphereShader);
      sphereShader = LoadShader(sphereShaderFileVS, sphereShaderFileFS);
      mat.shader = sphereShader;
      lastModified = currentModTime;
    }

    Vector3 lightPos = {3.0f, 5.0f, 3.0f};
    Vector3 lightPos2 = {-3.0f, -5.0f, -3.0f};
    Vector3 viewPos = cam.position;
    SetShaderValue(sphereShader, GetShaderLocation(sphereShader, "lightPos"),
                   &lightPos, SHADER_UNIFORM_VEC3);
    SetShaderValue(sphereShader, GetShaderLocation(sphereShader, "lightPos2"),
                   &lightPos2, SHADER_UNIFORM_VEC3);
    SetShaderValue(sphereShader, GetShaderLocation(sphereShader, "viewPos"),
                   &viewPos, SHADER_UNIFORM_VEC3);

    Vector4 color = {
        sphere._mesh.colors[0] / 255.0f, sphere._mesh.colors[1] / 255.0f,
        sphere._mesh.colors[2] / 255.0f, sphere._mesh.colors[3] / 255.0f};
    SetShaderValue(sphereShader, GetShaderLocation(sphereShader, "fragColor"),
                   &color, SHADER_UNIFORM_VEC4);

    camController.update(cam);

    BeginMode3D(cam);

    DrawMesh(sphere._mesh, mat, MatrixIdentity());

    rlEnableWireMode();

    DrawMesh(sphere._mesh, matWire,
             MatrixScale(1.002f, 1.002f, 1.002f)); // pase de líneas
    rlDisableWireMode();

    EndMode3D();

    EndDrawing();
  }

  UnloadShader(sphereShader);
  CloseWindow();

  return 0;
}