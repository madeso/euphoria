#include "spacetyper/vao.h"

Point::Point(float x, float y, float u, float v) : pos(x, y), vert(u, v) {}

Point::Point(const glm::vec2& apos, const glm::vec2& avert)
    : pos(apos), vert(avert) {}

VaoBuilder& VaoBuilder::vertex(const Point& p) {
  data.push_back(p.pos.x);
  data.push_back(p.pos.y);
  data.push_back(p.vert.x);
  data.push_back(p.vert.y);
  return *this;
}

VaoBuilder& VaoBuilder::triangle(const Point& p1, const Point& p2,
                                 const Point& p3) {
  vertex(p1);
  vertex(p2);
  vertex(p3);
  return *this;
}

VaoBuilder& VaoBuilder::quad(const Point& a, const Point& b, const Point& c,
                             const Point& d) {
  triangle(c, b, a);
  triangle(c, d, b);
  return *this;
}

Vao::Vao(const VaoBuilder& data) : count_(data.data.size() / 4) {
  GLuint VBO;

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.data.size(),
               &data.data[0], GL_STATIC_DRAW);

  glBindVertexArray(vao_);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
                        static_cast<GLvoid*>(nullptr));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

Vao::~Vao() { glDeleteVertexArrays(1, &vao_); }

void Vao::Draw() const {
  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLES, 0, count_);
  glBindVertexArray(0);
}