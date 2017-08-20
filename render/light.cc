#include "render/light.h"

Light::Light()
    : position_(0, 0, 0)
    , direction_(vec3f{-0.2f, -1.0f, -0.3f}.GetNormalized())
    , ambient_(0.3f)
    , diffuse_(Rgb::From(Color::White))
    , specular_(Rgb::From(Color::White))
{
}

void
Light::SetPosition(const vec3f& pos)
{
  position_ = pos;
}

const vec3f&
Light::GetPosition() const
{
  return position_;
}

void
Light::SetDirection(const vec3f::Unit& direction)
{
  direction_ = direction;
}

const vec3f::Unit&
Light::GetDirection() const
{
  return direction_;
}

void
Light::SetAmbient(const Rgb& c)
{
  ambient_ = c;
}

const Rgb&
Light::GetAmbient() const
{
  return ambient_;
}


Rgb*
Light::ModifyAmbient()
{
  return &ambient_;
}

void
Light::SetDiffuse(const Rgb& c)
{
  diffuse_ = c;
}

const Rgb&
Light::GetDiffuse() const
{
  return diffuse_;
}

Rgb*
Light::ModifyDiffuse()
{
  return &diffuse_;
}

void
Light::SetSpecular(const Rgb& c)
{
  specular_ = c;
}

const Rgb&
Light::GetSpecular() const
{
  return specular_;
}

Rgb*
Light::ModifySpecular()
{
  return &specular_;
}
