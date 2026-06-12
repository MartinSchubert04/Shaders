#pragma once

#include "pch.h"

class CameraController {

  enum class TouchState { Idle, Orbiting, Zooming };

public:
  CameraController() = default;
  void update(Camera3D &camera);

private:
  void handleMovement(Camera3D &camera);
  void handleZoom(Camera3D &camera);
  void handlePlanetSelection(Camera3D &camera);
  void handleCameraTarget(Camera3D &camera);
  Vector2 getTouchDelta();
  Vector2 getTouch();

private:
  Vector2 _lastPos = {0.f, 0.f};
  b32 _touching = false;
  float _lastPinchDist;
  TouchState _lastState = TouchState::Idle;
  bool _arrived = false;
  float _customDist;
};
