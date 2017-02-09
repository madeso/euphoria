#include "render/buffer.h"

#include <cassert>

#include "render/gl.h"
#include "render/shaderattribute.h"

Vbo::Vbo() { glGenBuffers(1, &id_); }

Vbo::~Vbo() { glDeleteBuffers(1, &id_); }

void Vbo::SetData(const std::vector<float>& data) {
  assert(this);
  assert(GetBound() == this);
  // use GL_DYNAMIC_DRAW or GL_STREAM_DRAW instead?
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), &data[0], GL_STATIC_DRAW);
}

void Vbo::Bind(Vbo* vbo){
  const gluint id = vbo ? vbo->id_ : 0;
  glBindBuffer(GL_ARRAY_BUFFER, id);
  GetBound() = vbo;
}

Vbo*& Vbo::GetBound(){
  static Vbo* vbo = nullptr;
  return vbo;
}

////////////////////////////////////////////////////////////////////////////////

Vao::Vao() { glGenVertexArrays(1, &id_); }

Vao::~Vao(){ glDeleteVertexArrays(1, &id_); }

void Vao::BindVboData(const ShaderAttribute& attribute, int stride, int offset)  {
  assert(this);
  assert(GetBound() == this);
  assert(Vbo::GetBound() == nullptr);
  int size = static_cast<int>(attribute.size);
  assert(size >= 1 && size <= 4);
  glVertexAttribPointer(attribute.id, size, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLvoid*>(offset));
  glEnableVertexAttribArray(attribute.id);
}

void Vao::Bind(Vao* vao) {
  const gluint id = vao ? vao->id_ : 0;
  glBindVertexArray(id);
  GetBound() = vao;
}

Vao*& Vao::GetBound() {
  static Vao* vao = nullptr;
  return vao;
}

////////////////////////////////////////////////////////////////////////////////

Ebo::Ebo() { glGenBuffers(1, &id_); }
Ebo::~Ebo() { glDeleteBuffers(1, &id_); }

void Ebo::SetData(const std::vector<unsigned int>& indices){
  assert(this);
  assert(GetBound() == this);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

void Ebo::Draw(int count) {
  assert(this);
  assert(GetBound() == this);
  glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
}

void Ebo::Bind(Ebo* ebo){
  const gluint id = ebo ? ebo->id_ : 0;
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
  GetBound() = ebo;
}

Ebo*& Ebo::GetBound() {
  static Ebo* ebo = nullptr;
  return ebo;
}

