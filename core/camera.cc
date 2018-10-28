#include "core/camera.h"

#include "core/assert.h"

CompiledCamera::CompiledCamera(const mat4f& view_, const mat4f& projection_)
    : view(view_)
    , projection(projection_)
    , combined(view_ * projection_)
    , combined_inverted(projection_ * view_)
{
  const bool was_inverted = combined_inverted.Invert();
  ASSERT(was_inverted);
}

point3f
CompiledCamera::WorldToClip(const point3f& in_world) const
{
  const vec4f v = combined * vec4f{in_world};

  // divide by w to get back to the 1.0f coordinate space
  return Scale(point3f{v.x, v.y, v.z}, 1 / v.w);
}

point3f
CompiledCamera::ClipToWorld(const point3f& in_clip) const
{
  const vec4f v = combined_inverted * vec4f{in_clip};

  // divide by w to get back to the 1.0f coordinate space
  return Scale(point3f{v.x, v.y, v.z}, 1 / v.w);
}

Ray3f
CompiledCamera::ClipToWorldRay(const point2f& p) const
{
  const auto from = ClipToWorld(point3f{p, -1.0f});
  const auto to   = ClipToWorld(point3f{p, 1.0f});
  return Ray3f::FromTo(from, to);
}

Camera::Camera()
    : position(point3f::Origo())
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
  return rotation.GetConjugate().ToMat4() *
         mat4f::FromTranslation(-static_cast<Vec3f>(position));
}
