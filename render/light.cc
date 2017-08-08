#include "render/light.h"

Light::Light()
  : position_(0,0,0)
  , color_(Rgb::From(Color::White))
  , ambient_(0.1f)
{}

void Light::SetPosition(const vec3f& pos)
{
  position_ = pos;
}

const vec3f& Light::GetPosition() const
{
  return position_;
}

void Light::SetColor(const Rgb& color)
{
  color_ = color;
}

const Rgb& Light::GetColor() const
{
  return color_;
}

void Light::SetAmbient(float amb)
{
  ambient_ = amb;
}

float Light::GetAmbient() const
{
  return ambient_;
}
