#include "render/bufferbuilder2d.h"


Point::Point(float x, float y, float u, float v) : pos(x, y), vert(u, v) {}

Point::Point(const vec2f& apos, const vec2f& avert)
    : pos(apos), vert(avert) {}


void BufferBuilder2d::AddVertex(const Point& p) {
  data.push_back(p.pos.x);
  data.push_back(p.pos.y);
  data.push_back(p.vert.x);
  data.push_back(p.vert.y);
  return *this;
}

void BufferBuilder2d::AddTriangle(unsigned int a, unsigned int b, unsigned int c) {
  tris.push_back(a);
  tris.push_back(b);
  tris.push_back(c);
}

void BufferBuilder2d::AddQuad(const Point& a, const Point& b, const Point& c,
             const Point& d) {
  AddVertex(a);
  AddVertex(b);
  AddVertex(c);
  AddVertex(d);

  const unsigned int ai = data.size() / 4;
  const unsgined int bi = ai + 1;
  const unsgined int ci = ai + 2;
  const unsgined int di = ai + 3;
  triangle(ci, bi, ai);
  triangle(ci, di, bi);
}
