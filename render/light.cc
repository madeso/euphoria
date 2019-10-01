#include "render/light.h"

namespace euphoria::render
{

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
    , position_( core::vec3f::Zero() )
    , direction_(core::vec3f{-0.2f, -1.0f, -0.3f}.GetNormalized())
    , ambient_(0.3f)
    , diffuse_(core::Color::White)
    , specular_(core::Color::White)
    , cutoffAngleOuter_(core::Angle::FromDegrees(18.0f))
    , cutoffAngleInner_(core::Angle::FromDegrees(13.37f))
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
Light::SetPosition(const core::vec3f& pos)
{
  position_ = pos;
}

const core::vec3f&
Light::GetPosition() const
{
  return position_;
}

void
Light::SetDirection(const core::unit3f& direction)
{
  direction_ = direction;
}

const core::unit3f&
Light::GetDirection() const
{
  return direction_;
}

void
Light::SetAmbient(const core::Rgb& c)
{
  ambient_ = c;
}

const core::Rgb&
Light::GetAmbient() const
{
  return ambient_;
}


core::Rgb*
Light::ModifyAmbient()
{
  return &ambient_;
}

void
Light::SetDiffuse(const core::Rgb& c)
{
  diffuse_ = c;
}

const core::Rgb&
Light::GetDiffuse() const
{
  return diffuse_;
}

core::Rgb*
Light::ModifyDiffuse()
{
  return &diffuse_;
}

void
Light::SetSpecular(const core::Rgb& c)
{
  specular_ = c;
}

const core::Rgb&
Light::GetSpecular() const
{
  return specular_;
}

core::Rgb*
Light::ModifySpecular()
{
  return &specular_;
}

const LightAttenuation&
Light::GetAttenuation() const
{
  return attenuation_;
}

const core::Angle&
Light::GetCutoffAngleOuter() const
{
  return cutoffAngleOuter_;
}

void
Light::SetCutoffAngleOuter(const core::Angle& angle)
{
  cutoffAngleOuter_ = angle;
}

core::Angle*
Light::GetCutoffAngleOuterMod()
{
  return &cutoffAngleOuter_;
}

const core::Angle&
Light::GetCutoffAngleInner() const
{
  return cutoffAngleInner_;
}

void
Light::SetCutoffAngleInner(const core::Angle& angle)
{
  cutoffAngleInner_ = angle;
}

core::Angle*
Light::GetCutoffAngleInnerMod()
{
  return &cutoffAngleInner_;
}

}
