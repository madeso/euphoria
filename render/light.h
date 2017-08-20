#ifndef EUPHORIA_LIGHT_H
#define EUPHORIA_LIGHT_H

#include "core/vec3.h"
#include "core/rgb.h"
#include "core/angle.h"

class LightAttenuation
{
 public:
  LightAttenuation();

  float
  GetConstant() const;

  float
  GetLinear() const;

  float
  GetQuadratic() const;

 private:
  float constant_;
  float linear_;
  float quadratic_;
};

class Light
{
 public:
  Light();

  enum class Type : int
  {
    Directional,
    Point
  };

  void
  SetType(Type type);
  Type
  GetType() const;
  Type*
  GetTypeMod();

  void
  SetPosition(const vec3f& pos);
  const vec3f&
  GetPosition() const;

  void
  SetDirection(const vec3f::Unit& direction);

  const vec3f::Unit&
  GetDirection() const;

  void
  SetAmbient(const Rgb& c);

  const Rgb&
  GetAmbient() const;

  Rgb*
  ModifyAmbient();

  void
  SetDiffuse(const Rgb& c);

  const Rgb&
  GetDiffuse() const;

  Rgb*
  ModifyDiffuse();

  void
  SetSpecular(const Rgb& c);

  const Rgb&
  GetSpecular() const;

  Rgb*
  ModifySpecular();

  const LightAttenuation&
  GetAttenuation() const;

  const Angle&
  GetCutoffAngleOuter() const;

  void
  SetCutoffAngleOuter(const Angle& angle);

  Angle*
  GetCutoffAngleOuterMod();

  const Angle&
  GetCutoffAngleInner() const;

  void
  SetCutoffAngleInner(const Angle& angle);

  Angle*
  GetCutoffAngleInnerMod();

 private:
  Type        type_;
  vec3f       position_;
  vec3f::Unit direction_;
  Rgb         ambient_;
  Rgb         diffuse_;
  Rgb         specular_;
  Angle       cutoffAngleOuter_;
  Angle       cutoffAngleInner_;

  LightAttenuation attenuation_;
};


#endif  // EUPHORIA_LIGHT_H
