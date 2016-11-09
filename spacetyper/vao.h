#ifndef SPACETYPER_VAO_H
#define SPACETYPER_VAO_H

#include <vector>
#include "glm/vec2.hpp"
#include "spacetyper/gl.h"
#include "spacetyper/noncopyable.h"

class Point {
 public:
  Point(float x, float y, float u, float v);
  Point(const glm::vec2& pos, const glm::vec2& vert);

  const glm::vec2 pos;
  const glm::vec2 vert;
};

class VaoBuilder {
 public:
  VaoBuilder& vertex(const Point& p);
  VaoBuilder& triangle(const Point& p1, const Point& p2, const Point& p3);
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
