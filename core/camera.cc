#include "core/camera.h"

#include "core/assert.h"

CompiledCamera::CompiledCamera(const mat4f& view_, const mat4f& projection_)
    : view(view_)
    , projection(projection_)
    , inverted_view(view_)
    , inverted_projection(projection_)
{
  const bool view_was_inverted       = inverted_view.Invert();
  const bool projection_was_inverted = inverted_projection.Invert();

  ASSERT(view_was_inverted);
  ASSERT(projection_was_inverted);
}

namespace
{
  const float POINT_VALUE = 0.0;
}

vec3f
CompiledCamera::WorldToClip(const vec3f& in_world) const
{
  mat4f       m       = projection * view;
  const vec4f in_view = m * vec4f{in_world, POINT_VALUE};
  return in_view.ToVec3();
}

vec3f
CompiledCamera::ClipToWorld(const vec3f& in_clip) const
{
  mat4f m = projection * view;
  m.Invert();
  const vec4f in_view = m * vec4f{in_clip, POINT_VALUE};
  return in_view.ToVec3();
}

Ray3f
CompiledCamera::ClipToWorldRay(const vec2f& p) const
{
  const auto from = ClipToWorld(vec3f{p, -1.0f});
  const auto to   = ClipToWorld(vec3f{p, 1.0f});
  return Ray3f::FromTo(from, to);
}

Camera::Camera()
    : position(vec3f::Origo())
    , rotation(quatf::Identity())
    , fov(45.0f)
    , near(0.1f)
    , far(100.0f)
{
}

CompiledCamera
Camera::Compile(float aspect) const
{
  return CompiledCamera{CalculateViewMatrix(),
                        CalculateProjectionMatrix(aspect)};
}

mat4f
Camera::CalculateProjectionMatrix(float aspect) const
{
  const mat4f projection_matrix =
      mat4f::Perspective(Angle::FromDegrees(fov), aspect, near, far);
  return projection_matrix;
}

mat4f
Camera::CalculateViewMatrix() const
{
  return rotation.GetConjugate().ToMat4() * mat4f::FromTranslation(-position);
}
