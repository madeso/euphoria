#include "render/camera.h"

Camera::Camera()
    : position_(vec3f::Origo())
    , rotation_(quatf::Identity())
    , fov_(45.0f)
    , near_(0.1f)
    , far_(100.0f)
{
}

const vec3f&
Camera::GetPosition() const
{
  return position_;
}

const quatf&
Camera::GetRotation() const
{
  return rotation_;
}

float
Camera::GetFov() const
{
  return fov_;
}

float
Camera::GetNear() const
{
  return near_;
}

float
Camera::GetFar() const
{
  return far_;
}

void
Camera::SetPosition(const vec3f& position)
{
  position_ = position;
}

void
Camera::SetRotation(const quatf& rotation)
{
  rotation_ = rotation;
}

void
Camera::SetFov(float fov)
{
  fov_ = fov;
}

void
Camera::SetNear(float near)
{
  near_ = near;
}

void
Camera::SetFar(float far)
{
  far_ = far;
}

mat4f
Camera::CalculateProjectionMatrix(float aspect) const
{
  const mat4f projection_matrix = mat4f::Perspective(
      Angle::FromDegrees(GetFov()), aspect, GetNear(), GetFar());
  return projection_matrix;
}

mat4f
Camera::CalculateViewMatrix() const
{
  return rotation_.GetConjugate().ToMat4() * mat4f::FromTranslation(-position_);
}
