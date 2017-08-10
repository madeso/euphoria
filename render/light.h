#ifndef EUPHORIA_LIGHT_H
#define EUPHORIA_LIGHT_H

#include "core/vec3.h"
#include "core/rgb.h"

class Light {
public:
  Light();

  void SetPosition(const vec3f& pos);
  const vec3f& GetPosition() const;

  void SetAmbient(const Rgb& c);
  const Rgb& GetAmbient() const;

  void SetDiffuse(const Rgb& c);
  const Rgb& GetDiffuse() const;

  void SetSpecular(const Rgb& c);
  const Rgb& GetSpecular() const;
private:
  vec3f position_;
  Rgb ambient_;
  Rgb diffuse_;
  Rgb specular_;
};


#endif //EUPHORIA_LIGHT_H
