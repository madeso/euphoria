#ifndef RENDER_BUFFERBUILDER2D_H
#define RENDER_BUFFERBUILDER2D_H

class Vao;
class Ebo;

#include <vector>

#include "core/vec2.h"

class Point {
 public:
  Point(float x, float y, float u, float v);
  Point(const vec2f& pos, const vec2f& vert);

  const vec2f pos;
  const vec2f vert;
};

class BufferBuilder2d {
 public:
  BufferBuilder2d();

  void AddVertex(const Point& p);
  void AddTriangle(unsigned int a, unsigned int b, unsigned int c);
  void AddQuad(const Point& a, const Point& b, const Point& c,
                   const Point& d);

  void SetupVao(Vao* vao) const;
  void SetupEbo(Ebo* ebo) const;
 private:
  std::vector<float> data;
  std::vector<unsigned int> tris;
};

#endif  // RENDER_BUFFERBUILDER2D_H
