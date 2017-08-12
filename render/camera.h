#ifndef EUPHORIA_CAMERA_H
#define EUPHORIA_CAMERA_H

#include "core/mat4.h"
#include "core/vec3.h"
#include "core/quat.h"

class Camera
{
 public:
  Camera();

  const vec3f&
  GetPosition() const;
  const quatf&
  GetRotation() const;
  float
  GetFov() const;
  float
  GetNear() const;
  float
  GetFar() const;

  void
  SetPosition(const vec3f& position);
  void
  SetRotation(const quatf& rotation);
  void
  SetFov(float fov);
  void
  SetNear(float near);
  void
  SetFar(float far);

  mat4f
  CalculateProjectionMatrix(float aspect) const;

  mat4f
  CalculateViewMatrix() const;

 private:
  vec3f position_;
  quatf rotation_;
  float fov_;
  float near_;
  float far_;
};


#endif  // EUPHORIA_CAMERA_H
