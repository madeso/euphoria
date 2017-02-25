#include "render/bufferbuilder2d.h"


Point::Point(float x, float y, float u, float v) : pos(x, y), vert(u, v) {}

Point::Point(const vec2f& apos, const vec2f& avert)
    : pos(apos), vert(avert) {}

BufferBuilder2d::BufferBuilder2d() {}

void BufferBuilder2d::AddVertex(const Point& p) {
  Assert(this);
  data.push_back(p.pos.x);
  data.push_back(p.pos.y);
  data.push_back(p.vert.x);
  data.push_back(p.vert.y);
}

void BufferBuilder2d::AddTriangle(unsigned int a, unsigned int b, unsigned int c) {
  Assert(this);
  tris.push_back(a);
  tris.push_back(b);
  tris.push_back(c);
}

void BufferBuilder2d::AddQuad(const Point& a, const Point& b, const Point& c,
             const Point& d) {
  Assert(this);
  AddVertex(a);
  AddVertex(b);
  AddVertex(c);
  AddVertex(d);

  const unsigned int ai = data.size() / 4;
  const unsigned int bi = ai + 1;
  const unsigned int ci = ai + 2;
  const unsigned int di = ai + 3;
  AddTriangle(ci, bi, ai);
  AddTriangle(ci, di, bi);
}

const std::vector<float>& BufferBuilder2d::GetVertexData() const {
  Assert(this);
  return data;
}

const std::vector<unsigned int>& BufferBuilder2d::GetTriangleIndices() const {
  Assert(this);
  return tris;
}
