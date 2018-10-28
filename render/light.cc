#include "render/light.h"

LightAttenuation::LightAttenuation()
    : constant_(1.0f)
    , linear_(0.09f)
    , quadratic_(0.032f)
{
}

float
LightAttenuation::GetConstant() const
{
  return constant_;
}

float
LightAttenuation::GetLinear() const
{
  return linear_;
}

float
LightAttenuation::GetQuadratic() const
{
  return quadratic_;
}

////////////////////////////////////////////////////////////////////////////////

Light::Light()
    : type_(Type::Directional)
    , position_(0, 0, 0)
    , direction_(vec3f{-0.2f, -1.0f, -0.3f}.GetNormalized())
    , ambient_(0.3f)
    , diffuse_(Color::White)
    , specular_(Color::White)
    , cutoffAngleOuter_(Angle::FromDegrees(18.0f))
    , cutoffAngleInner_(Angle::FromDegrees(13.37f))
{
}

void
Light::SetType(Type type)
{
  type_ = type;
}

Light::Type
Light::GetType() const
{
  return type_;
}

Light::Type*
Light::GetTypeMod()
{
  return &type_;
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
Light::SetDirection(const unit3f& direction)
{
  direction_ = direction;
}

const unit3f&
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

const LightAttenuation&
Light::GetAttenuation() const
{
  return attenuation_;
}

const Angle&
Light::GetCutoffAngleOuter() const
{
  return cutoffAngleOuter_;
}

void
Light::SetCutoffAngleOuter(const Angle& angle)
{
  cutoffAngleOuter_ = angle;
}

Angle*
Light::GetCutoffAngleOuterMod()
{
  return &cutoffAngleOuter_;
}

const Angle&
Light::GetCutoffAngleInner() const
{
  return cutoffAngleInner_;
}

void
Light::SetCutoffAngleInner(const Angle& angle)
{
  cutoffAngleInner_ = angle;
}

Angle*
Light::GetCutoffAngleInnerMod()
{
  return &cutoffAngleInner_;
}
