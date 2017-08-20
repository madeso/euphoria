#ifndef EUPHORIA_LIGHT_H
#define EUPHORIA_LIGHT_H

#include "core/vec3.h"
#include "core/rgb.h"

class Light
{
 public:
  Light();

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

 private:
  vec3f       position_;
  vec3f::Unit direction_;
  Rgb         ambient_;
  Rgb         diffuse_;
  Rgb         specular_;
};


#endif  // EUPHORIA_LIGHT_H
