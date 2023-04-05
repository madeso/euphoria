#include "render/buffer.h"

#include <algorithm>

#include "assert/assert.h"
#include "log/log.h"

#include "render/gl.h"
#include "render/shader.h"

#include "euph_generated_config.h"


namespace euphoria::render
{
    VertexBuffer::VertexBuffer()
    {
        glGenBuffers(1, &id);
    }


    VertexBuffer::~VertexBuffer()
    {
        glDeleteBuffers(1, &id);
    }


    void
    VertexBuffer::set_data(const std::vector<float>& data)
    {
        ASSERT(get_bound() == this);
        // use GL_DYNAMIC_DRAW or GL_STREAM_DRAW instead?
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(float) * data.size()), data.data(), GL_STATIC_DRAW);
    }


    void
    VertexBuffer::bind(const VertexBuffer* vbo)
    {
        const gl::Uint id = vbo != nullptr ? vbo->id : 0;
        glBindBuffer(GL_ARRAY_BUFFER, id);
        get_bound() = vbo;
    }


    const VertexBuffer*&
    VertexBuffer::get_bound()
    {
        static const VertexBuffer* vbo = nullptr;
        return vbo;
    }


    ////////////////////////////////////////////////////////////////////////////////


    PointLayout::PointLayout()
    {
        glGenVertexArrays(1, &id);
    }


    PointLayout::~PointLayout()
    {
        glDeleteVertexArrays(1, &id);
    }


    namespace
    {
        GLenum
        con(ShaderAttributeType type)
        {
            switch(type)
            {
            case ShaderAttributeType::float1:
            case ShaderAttributeType::float2:
            case ShaderAttributeType::float3:
            case ShaderAttributeType::float4:
            case ShaderAttributeType::float33:
            case ShaderAttributeType::float44: return GL_FLOAT;
            default: LOG_ERROR("Unhandled shader type"); return GL_FLOAT;
            }
        }
    }


    namespace
    {
        void* offset_to_pointer_offset(int aoffset)
        {
            // reinterpret_cast is probably ok since the void* is an offset
            // and not a actual pointer

            // use arch detection to store in a (potentially) bigger integer before converting to a pointer
#if EUPH_ARCH_32 == 1
            std::int32_t
#elif EUPH_ARCH_64 == 1
            std::int64_t
#else
    #error unknown arch
#endif
                offset = aoffset;
            return reinterpret_cast<GLvoid*>(offset); // NOLINT
        }
    }


    void
    PointLayout::bind_data(const ShaderAttribute& attribute, int stride, int offset)
    {
        ASSERT(get_bound() == this);
        ASSERT(VertexBuffer::get_bound() != nullptr);
        glVertexAttribPointer
        (
            attribute.id,
            attribute.get_element_count(),
            con(attribute.type),
            attribute.normalize ? GL_TRUE : GL_FALSE,
            stride,
            offset_to_pointer_offset(offset)
        );
        glEnableVertexAttribArray(attribute.id);

        attributes.push_back(attribute);
    }


    void
    PointLayout::bind(const PointLayout* vao)
    {
        const gl::Uint id = vao != nullptr ? vao->id : 0;
        glBindVertexArray(id);
        get_bound() = vao;
    }


    const PointLayout*&
    PointLayout::get_bound()
    {
        static const PointLayout* vao = nullptr;
        return vao;
    }


    ////////////////////////////////////////////////////////////////////////////////


    IndexBuffer::IndexBuffer()
    {
        glGenBuffers(1, &id);
    }


    IndexBuffer::~IndexBuffer()
    {
        glDeleteBuffers(1, &id);
    }


    void
    IndexBuffer::set_data(const std::vector<unsigned int>& indices)
    {
        ASSERT(get_bound() == this);
        glBufferData
        (
            GL_ELEMENT_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
            indices.data(),
            GL_STATIC_DRAW
        );
    }


    void
    IndexBuffer::draw(RenderMode mode, int count) const
    {
        ASSERT(PointLayout::get_bound() != nullptr);
        ASSERT(ShaderProgram::get_current_bound_for_debug() != nullptr);

        const PointLayout* vao = PointLayout::get_bound();
        const ShaderProgram* shader = ShaderProgram::get_current_bound_for_debug();

        ASSERT(vao);
        ASSERT(shader);

        ASSERTX(get_bound() == this, static_cast<const void*>(get_bound()), static_cast<const void*>(this));

        const auto& a = shader->get_attributes();
        for(const auto& attribute: vao->attributes)
        {
            const bool found_in_shader = std::find(a.begin(), a.end(), attribute) != a.end();
            if(!found_in_shader)
            {
                LOG_ERROR
                (
                    "Failed to find attribute {0} bound in shader {1}",
                    attribute.name,
                    shader->get_name()
                );
                ASSERT(found_in_shader);
            }
        }

        if(mode == RenderMode::triangles)
        {
            glDrawElements(GL_TRIANGLES, count * 3, GL_UNSIGNED_INT, nullptr);
        }
        else
        {
            glDrawElements(GL_LINES, count * 2, GL_UNSIGNED_INT, nullptr);
        }
    }

    void
    IndexBuffer::bind(const IndexBuffer* ebo)
    {
        const gl::Uint id = ebo != nullptr ? ebo->id : 0;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        get_bound() = ebo;
    }

    const IndexBuffer*&
    IndexBuffer::get_bound()
    {
        static const IndexBuffer* ebo = nullptr;
        return ebo;
    }
}
