#ifndef RENDER_BUFFER_H
#define RENDER_BUFFER_H

#include <vector>

#include "render/id.h"
#include "render/shaderattribute.h"

class Vbo : public Id {
 public:
  Vbo();
  ~Vbo();

  void SetData(const std::vector<float>& data);

  static void Bind(const Vbo* vbo);
  static const Vbo*& GetBound();
};

class Vao : public Id {
 public:
  Vao();
  ~Vao();

  void BindVboData(const ShaderAttribute& attribute, int stride, int offset);

  static void Bind(const Vao* vao);
  static const Vao*& GetBound();

  // debug
  std::vector<ShaderAttribute> attributes;
};

class Ebo : public Id {
 public:
  Ebo();
  ~Ebo();

  void SetData(const std::vector<unsigned int>& indices);
  void Draw(int count) const;

  static void Bind(const Ebo* ebo);
  static const Ebo*& GetBound();
};



#endif  // RENDER_BUFFER_H
