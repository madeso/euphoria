#ifndef EUPHORIA_LIGHT_H
#define EUPHORIA_LIGHT_H

#include "core/vec3.h"
#include "core/rgb.h"

class Light {
public:
  Light();

  void SetPosition(const vec3f& pos);
  const vec3f& GetPosition() const;

  void SetColor(const Rgb& color);
  const Rgb& GetColor() const;

  void SetAmbient(float amb);
  float GetAmbient() const;
private:
  vec3f position_;
  Rgb color_;
  float ambient_;
};


#endif //EUPHORIA_LIGHT_H
