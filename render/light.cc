#include "render/light.h"

Light::Light()
  : position_(0,0,0)
  , ambient_(0.1f)
  , diffuse_(Rgb::From(Color::White))
  , specular_(Rgb::From(Color::White))
{}

void Light::SetPosition(const vec3f& pos)
{
  position_ = pos;
}

const vec3f& Light::GetPosition() const
{
  return position_;
}

void Light::SetAmbient(const Rgb& c)
{
  ambient_ = c;
}
const Rgb& Light::GetAmbient() const
{
  return ambient_;
}

void Light::SetDiffuse(const Rgb& c)
{
  diffuse_ = c;
}
const Rgb& Light::GetDiffuse() const
{
  return diffuse_;
}

void Light::SetSpecular(const Rgb& c)
{
  specular_ = c;
}

const Rgb& Light::GetSpecular() const
{
  return specular_;
}
