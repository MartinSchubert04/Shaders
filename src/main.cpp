#include "raylib.h"

int main() {
  InitWindow(1200, 1000, "Hello");

  auto shader = LoadShader("assets/shader.vs", "assets/shader.fs");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    SetWindowTitle(TextFormat("GLSL shaders | fps: %d", GetFPS()));

    int resPos = GetShaderLocation(shader, "iResolution");
    Vector3 res = {(float)GetScreenWidth(), (float)GetScreenHeight(), 0.0f};
    SetShaderValue(shader, resPos, &res, SHADER_UNIFORM_VEC3);

    int timeLoc = GetShaderLocation(shader, "iTime");
    float time = GetTime();
    SetShaderValue(shader, timeLoc, &time, SHADER_UNIFORM_FLOAT);

    BeginShaderMode(shader);
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);
    EndShaderMode();

    EndDrawing();
  }

  UnloadShader(shader);
  CloseWindow();

  return 0;
}