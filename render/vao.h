#ifndef SPACETYPER_VAO_H
#define SPACETYPER_VAO_H

#include <vector>
#include "core/vec2.h"
#include "render/gl.h"
#include "core/noncopyable.h"

class Point {
 public:
  Point(float x, float y, float u, float v);
  Point(const vec2f& pos, const vec2f& vert);

  const vec2f pos;
  const vec2f vert;
};

class VaoBuilder {
 public:
  VaoBuilder& vertex(const Point& p);

  // clockwise
  VaoBuilder& triangle(const Point& p1, const Point& p2, const Point& p3);

  // clockwise
  VaoBuilder& quad(const Point& a, const Point& b, const Point& c,
                   const Point& d);

  std::vector<GLfloat> data;
};

class Vao : Noncopyable {
 public:
  Vao(const VaoBuilder& data);
  ~Vao();

  void Draw() const;

 private:
  GLuint vao_;
  unsigned long count_;
};

#endif  // SPACETYPER_VAO_H
