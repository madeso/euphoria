#include "render/buffer.h"

#include "core/assert.h"

#include "render/gl.h"
#include "render/shader.h"
#include <algorithm>

Vbo::Vbo()
{
  glGenBuffers(1, &id_);
}

Vbo::~Vbo()
{
  glDeleteBuffers(1, &id_);
}

void
Vbo::SetData(const std::vector<float>& data)
{
  Assert(GetBound() == this);
  // use GL_DYNAMIC_DRAW or GL_STREAM_DRAW instead?
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), &data[0],
               GL_STATIC_DRAW);
}

void
Vbo::Bind(const Vbo* vbo)
{
  const gluint id = vbo != nullptr ? vbo->id_ : 0;
  glBindBuffer(GL_ARRAY_BUFFER, id);
  GetBound() = vbo;
}

const Vbo*&
Vbo::GetBound()
{
  static const Vbo* s_Vbo = nullptr;
  return s_Vbo;
}

////////////////////////////////////////////////////////////////////////////////

Vao::Vao()
{
  glGenVertexArrays(1, &id_);
}

Vao::~Vao()
{
  glDeleteVertexArrays(1, &id_);
}

void
Vao::BindVboData(const ShaderAttribute& attribute, int stride, int offset)
{
  Assert(GetBound() == this);
  Assert(Vbo::GetBound() != nullptr);
  auto size = static_cast<int>(attribute.size);
  Assert(size >= 1 && size <= 4);
  // reinterpret_cast is probably ok since the void* is an offset
  // and not a actual pointer
  glVertexAttribPointer(attribute.id, size, GL_FLOAT, GL_FALSE, stride,
                        reinterpret_cast<GLvoid*>(offset));  // NOLINT
  glEnableVertexAttribArray(attribute.id);

  attributes.push_back(attribute);
}

void
Vao::Bind(const Vao* vao)
{
  const gluint id = vao != nullptr ? vao->id_ : 0;
  glBindVertexArray(id);
  GetBound() = vao;
}

const Vao*&
Vao::GetBound()
{
  static const Vao* s_Vao = nullptr;
  return s_Vao;
}

////////////////////////////////////////////////////////////////////////////////

Ebo::Ebo()
{
  glGenBuffers(1, &id_);
}
Ebo::~Ebo()
{
  glDeleteBuffers(1, &id_);
}

void
Ebo::SetData(const std::vector<unsigned int>& indices)
{
  Assert(GetBound() == this);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               &indices[0], GL_STATIC_DRAW);
}

void
Ebo::Draw(int count) const
{
  Assert(Vao::GetBound() != nullptr);
  Assert(Shader::CurrentlyBound() != nullptr);

  const Vao*    vao    = Vao::GetBound();
  const Shader* shader = Shader::CurrentlyBound();
  Assert(vao);
  Assert(shader);

  const auto& a = shader->GetAttributes();
  for(const auto& attribute : vao->attributes)
  {
    const bool found_in_shader =
        std::find(a.begin(), a.end(), attribute) != a.end();
    if(!found_in_shader)
    {
      std::cerr << "Failed to find attribute " << attribute.name
                << " bound in shader " << shader->GetName() << "\n";
      Assert(found_in_shader);
    }
  }


  glDrawElements(GL_TRIANGLES, count * 3, GL_UNSIGNED_INT, nullptr);
}

void
Ebo::Bind(const Ebo* ebo)
{
  const gluint id = ebo != nullptr ? ebo->id_ : 0;
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
  GetBound() = ebo;
}

const Ebo*&
Ebo::GetBound()
{
  static const Ebo* s_Ebo = nullptr;
  return s_Ebo;
}
