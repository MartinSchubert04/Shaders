#include "CameraController.h"
#include "raylib.h"

void CameraController::update(Camera3D &camera) {
  handleCameraTarget(camera);

  TouchState state = GetMouseWheelMove() != 0 ? TouchState::Zooming
                     : IsMouseButtonDown(MOUSE_BUTTON_LEFT)
                         ? TouchState::Orbiting
                         : TouchState::Idle;

  if (state != _lastState && state == TouchState::Orbiting) {
    handlePlanetSelection(camera);
  }

  switch (state) {
  case TouchState::Idle:
    _touching = false;
    _lastPinchDist = 0.f;
    break;

  case TouchState::Orbiting:
    handleMovement(camera);
    _touching = true;
    break;

  case TouchState::Zooming:
    handleZoom(camera);
    _touching = false;
    break;
  }

  _lastState = state;
}

Vector2 CameraController::getTouch() {
  return {(float)GetTouchX(), (float)GetTouchY()};
}
Vector2 CameraController::getTouchDelta() {
  Vector2 pos = getTouch();
  Vector2 delta = {pos.x - _lastPos.x, pos.y - _lastPos.y};
  _lastPos = pos;
  return delta;
}

void CameraController::handleMovement(Camera3D &camera) {
  Vector2 delta = GetMouseDelta();

  if (_touching) {
    float angleH = delta.x * 0.005f;
    float angleV = delta.y * 0.005f;

    Vector3 offset = Vector3Subtract(camera.position, camera.target);

    offset = Vector3RotateByAxisAngle(offset, {0, 1, 0}, -angleH);

    Vector3 right = Vector3Normalize(Vector3CrossProduct(offset, camera.up));
    Vector3 candidate = Vector3RotateByAxisAngle(offset, right, angleV);

    // solo aceptar si el candidato no supera ~85° del polo (evita el flip)
    float polarCos =
        fabsf(Vector3DotProduct(Vector3Normalize(candidate), {0, 1, 0}));
    if (polarCos < 0.996f)
      offset = candidate;

    camera.position = Vector3Add(camera.target, offset);
  }
}

void CameraController::handleZoom(Camera3D &camera) {
  float zoomScale = .5f;

  Vector3 dir =
      Vector3Normalize(Vector3Subtract(camera.position, camera.target));
  camera.position = Vector3Add(
      camera.position, Vector3Scale(dir, -GetMouseWheelMove() * zoomScale));
}

void CameraController::handlePlanetSelection(Camera3D &camera) {}

void CameraController::handleCameraTarget(Camera3D &camera) {}