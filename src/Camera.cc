#include "Camera.hh"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <SDL2/SDL.h>
#include "Util.hh"

#include <iostream>
using namespace std;

Camera::Camera() {
  proj_ = perspective(
    45.0f, Util::U().GetWidth() / (float)Util::U().GetHeight(), 0.01f, 10000.0f);

  switch (Util::mode_) {
    case Util::kNormalMode:
      heading_ = vec3(0, 0, 1);
      position_ = vec3(2, 3, 0);
      break;
    case Util::kBoidsMode:
      heading_ = vec3(0.000300482, -1.41443, -0.000270552);
      position_ = vec3(2, 100, 0);
      break;
  }
}

const mat4 Camera::GetView() {
  return lookAt(position_, position_ + heading_, up_);
}

const mat4 Camera::GetProj() {
  return proj_;
}

const vec3 Camera::GetPos() {
  return position_;
}

const vec3 Camera::GetHeading() {
  return heading_;
}

void Camera::Update(int dt) {
  int x, y;
  SDL_GetMouseState(&x, &y);

  float dx = (lastx_ - x) * dt / 15000.0f;
  float dy = (lasty_ - y) * dt / 30000.0f;
  if (x == 0) {
    dx = dt / 400.0f;
  }
  if (y == 0) {
    dy = dt / 1200.0f;
  }
  if (x == Util::U().GetWidth() - 1) {
    dx = dt / -400.0f;
  }
  if (y == Util::U().GetHeight() - 1) {
    dy = dt / -1200.0f;
  }

  switch (Util::mode_) {
    case Util::kNormalMode:
      Rotate(dx, dy);
      break;
  }

  lastx_ = x;
  lasty_ = y;

  const Uint8* state = SDL_GetKeyboardState(NULL);

  float delta = dt / 100.0f;
  if (state[SDL_SCANCODE_W]) {
    Translate(delta, 0, 0);
  }
  if (state[SDL_SCANCODE_A]) {
    Translate(0, delta, 0);
  }
  if (state[SDL_SCANCODE_S]) {
    Translate(-delta, 0, 0);
  }
  if (state[SDL_SCANCODE_D]) {
    Translate(0, -delta, 0);
  }
  if (state[SDL_SCANCODE_R]) {
    Translate(0, 0, delta);
  }
  if (state[SDL_SCANCODE_F]) {
    Translate(0, 0, -delta);
  }

  switch (Util::mode_) {
    case Util::kNormalMode:
      position_.y = 3;
      break;
  }
}

void Camera::Rotate(float dx, float dy) {
  quat yaw = angleAxis(dx, up_);
  quat pitch = angleAxis(dy, cross(heading_, up_));
  mat4 rotation =  mat4_cast(yaw * pitch);

  heading_ = mat3(rotation) * heading_;
}

void Camera::Translate(float forward, float strafe, float altitude) {
  position_ += forward * heading_;
  position_ += strafe * cross(up_, heading_);
  position_ += altitude * up_;
}
