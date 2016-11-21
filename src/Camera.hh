#ifndef CAMERA_HH
#define CAMERA_HH

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "Model.hh"

using namespace glm;

class Camera {
 public:
  Camera();

  const mat4 GetView();
  const mat4 GetProj();
  const vec3 GetPos();
  const vec3 GetHeading();

  void Update(int dt);

  void Rotate(float dx, float dy);

  void Translate(float forward, float strafe, float altitude);

 private:
  mat4 proj_;
  vec3 heading_;
  vec3 position_;
  vec3 up_ = vec3(0, 1.0f, 0);
  int lastx_ = 0, lasty_ = 0;
};

#endif
