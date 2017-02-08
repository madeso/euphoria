#ifndef RENDER_BUFFER_H
#define RENDER_BUFFER_H

#include <vector>

#include "render/id.h"

class Attribute;

class Vbo : public Id {
 public:
  Vbo();
  ~Vbo();

  void SetData(const std::vector<float>& data);

  static void Bind(Vbo* vbo);
  static Vbo*& GetBound();
};

class Vao : public Id {
 public:
  Vao();
  ~Vao();

  void BindVboData(const Attribute& attribute, int stride, int offset);

  static void Bind(Vao* vao);
  static Vao*& GetBound();
};

class Ebo : public Id {
 public:
  Ebo();
  ~Ebo();

  void SetData(const std::vector<unsigned int>& indices);
  void Draw(int count);

  static void Bind(Ebo* ebo);
  static Ebo*& GetBound();
};



#endif  // RENDER_BUFFER_H
