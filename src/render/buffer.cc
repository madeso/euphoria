#include "render/buffer.h"

#include "core/assert.h"
#include "core/log.h"

#include "render/gl.h"
#include "render/shader.h"
#include <algorithm>

namespace euphoria::render
{
    LOG_SPECIFY_DEFAULT_LOGGER("render.buffer")

    VertexBuffer::VertexBuffer()
    {
        glGenBuffers(1, &id_);
    }

    VertexBuffer::~VertexBuffer()
    {
        glDeleteBuffers(1, &id_);
    }

    void
    VertexBuffer::SetData(const std::vector<float>& data)
    {
        ASSERT(GetBound() == this);
        // use GL_DYNAMIC_DRAW or GL_STREAM_DRAW instead?
        glBufferData(
                GL_ARRAY_BUFFER,
                sizeof(float) * data.size(),
                &data[0],
                GL_STATIC_DRAW);
    }

    void
    VertexBuffer::Bind(const VertexBuffer* vbo)
    {
        const gluint id = vbo != nullptr ? vbo->id_ : 0;
        glBindBuffer(GL_ARRAY_BUFFER, id);
        GetBound() = vbo;
    }

    const VertexBuffer*&
    VertexBuffer::GetBound()
    {
        static const VertexBuffer* Vbo = nullptr;
        return Vbo;
    }

    ////////////////////////////////////////////////////////////////////////////////

    PointLayout::PointLayout()
    {
        glGenVertexArrays(1, &id_);
    }

    PointLayout::~PointLayout()
    {
        glDeleteVertexArrays(1, &id_);
    }

    namespace
    {
        GLenum
        GetOpenGLType(ShaderAttributeType type)
        {
            switch(type)
            {
            case ShaderAttributeType::FLOAT1:
            case ShaderAttributeType::FLOAT2:
            case ShaderAttributeType::FLOAT3:
            case ShaderAttributeType::FLOAT4:
            case ShaderAttributeType::FLOAT33:
            case ShaderAttributeType::FLOAT44: return GL_FLOAT;
            default: LOG_ERROR("Unhandled shader type"); return GL_FLOAT;
            }
        }
    }  // namespace

    void
    PointLayout::BindData(
            const ShaderAttribute& attribute,
            int                    stride,
            int                    offset)
    {
        ASSERT(GetBound() == this);
        ASSERT(VertexBuffer::GetBound() != nullptr);
        // reinterpret_cast is probably ok since the void* is an offset
        // and not a actual pointer
        glVertexAttribPointer(
                attribute.id,
                attribute.GetElementCount(),
                GetOpenGLType(attribute.type),
                attribute.normalize ? GL_TRUE : GL_FALSE,
                stride,
                reinterpret_cast<GLvoid*>(offset));  // NOLINT
        glEnableVertexAttribArray(attribute.id);

        attributes.push_back(attribute);
    }

    void
    PointLayout::Bind(const PointLayout* vao)
    {
        const gluint id = vao != nullptr ? vao->id_ : 0;
        glBindVertexArray(id);
        GetBound() = vao;
    }

    const PointLayout*&
    PointLayout::GetBound()
    {
        static const PointLayout* Vao = nullptr;
        return Vao;
    }

    ////////////////////////////////////////////////////////////////////////////////

    IndexBuffer::IndexBuffer()
    {
        glGenBuffers(1, &id_);
    }
    IndexBuffer::~IndexBuffer()
    {
        glDeleteBuffers(1, &id_);
    }

    void
    IndexBuffer::SetData(const std::vector<unsigned int>& indices)
    {
        ASSERT(GetBound() == this);
        glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                indices.size() * sizeof(unsigned int),
                &indices[0],
                GL_STATIC_DRAW);
    }

    void
    IndexBuffer::Draw(RenderMode mode, int count) const
    {
        ASSERT(PointLayout::GetBound() != nullptr);
        ASSERT(Shader::CurrentlyBound() != nullptr);

        const PointLayout* vao    = PointLayout::GetBound();
        const Shader*      shader = Shader::CurrentlyBound();
        ASSERT(vao);
        ASSERT(shader);

        const auto& a = shader->GetAttributes();
        for(const auto& attribute: vao->attributes)
        {
            const bool found_in_shader
                    = std::find(a.begin(), a.end(), attribute) != a.end();
            if(!found_in_shader)
            {
                LOG_ERROR
                (
                    "Failed to find attribute {0} bound in shader {1}",
                    attribute.name,
                    shader->GetName()
                );
                ASSERT(found_in_shader);
            }
        }

        if(mode == RenderMode::Triangles)
        {
            glDrawElements(GL_TRIANGLES, count * 3, GL_UNSIGNED_INT, nullptr);
        }
        else
        {
            glDrawElements(GL_LINES, count * 2, GL_UNSIGNED_INT, nullptr);
        }
    }

    void
    IndexBuffer::Bind(const IndexBuffer* ebo)
    {
        const gluint id = ebo != nullptr ? ebo->id_ : 0;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        GetBound() = ebo;
    }

    const IndexBuffer*&
    IndexBuffer::GetBound()
    {
        static const IndexBuffer* Ebo = nullptr;
        return Ebo;
    }

}  // namespace euphoria::render
