#include "render/buffer.h"

#include "core/assert.h"
#include "core/log.h"

#include "render/gl.h"
#include "render/shader.h"
#include <algorithm>

namespace euphoria::render
{
    vertex_buffer::vertex_buffer()
    {
        glGenBuffers(1, &id_);
    }


    vertex_buffer::~vertex_buffer()
    {
        glDeleteBuffers(1, &id_);
    }


    void
    vertex_buffer::set_data(const std::vector<float>& data)
    {
        ASSERT(get_bound() == this);
        // use GL_DYNAMIC_DRAW or GL_STREAM_DRAW instead?
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(float) * data.size()), &data[0], GL_STATIC_DRAW);
    }


    void
    vertex_buffer::bind(const vertex_buffer* vbo)
    {
        const gluint id = vbo != nullptr ? vbo->id_ : 0;
        glBindBuffer(GL_ARRAY_BUFFER, id);
        get_bound() = vbo;
    }


    const vertex_buffer*&
    vertex_buffer::get_bound()
    {
        static const vertex_buffer* vbo = nullptr;
        return vbo;
    }


    ////////////////////////////////////////////////////////////////////////////////


    point_layout::point_layout()
    {
        glGenVertexArrays(1, &id_);
    }


    point_layout::~point_layout()
    {
        glDeleteVertexArrays(1, &id_);
    }


    namespace
    {
        GLenum
        con(shader_attribute_type type)
        {
            switch(type)
            {
            case shader_attribute_type::float1:
            case shader_attribute_type::float2:
            case shader_attribute_type::float3:
            case shader_attribute_type::float4:
            case shader_attribute_type::float33:
            case shader_attribute_type::float44: return GL_FLOAT;
            default: LOG_ERROR("Unhandled shader type"); return GL_FLOAT;
            }
        }
    }


    void
    point_layout::bind_data(const shader_attribute& attribute, int stride, int offset)
    {
        ASSERT(get_bound() == this);
        ASSERT(vertex_buffer::get_bound() != nullptr);
        // reinterpret_cast is probably ok since the void* is an offset
        // and not a actual pointer
        glVertexAttribPointer
        (
                attribute.id,
                attribute.get_element_count(),
                con(attribute.type),
            attribute.normalize ? GL_TRUE : GL_FALSE,
                stride,
                reinterpret_cast<GLvoid*>(offset) // NOLINT
        );
        glEnableVertexAttribArray(attribute.id);

        attributes.push_back(attribute);
    }


    void
    point_layout::bind(const point_layout* vao)
    {
        const gluint id = vao != nullptr ? vao->id_ : 0;
        glBindVertexArray(id);
        get_bound() = vao;
    }


    const point_layout*&
    point_layout::get_bound()
    {
        static const point_layout* vao = nullptr;
        return vao;
    }


    ////////////////////////////////////////////////////////////////////////////////


    index_buffer::index_buffer()
    {
        glGenBuffers(1, &id_);
    }


    index_buffer::~index_buffer()
    {
        glDeleteBuffers(1, &id_);
    }


    void
    index_buffer::set_data(const std::vector<unsigned int>& indices)
    {
        ASSERT(get_bound() == this);
        glBufferData
        (
            GL_ELEMENT_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
            &indices[0],
            GL_STATIC_DRAW
        );
    }


    void
    index_buffer::draw(render_mode mode, int count) const
    {
        ASSERT(point_layout::get_bound() != nullptr);
        ASSERT(shader::get_current_bound_for_debug() != nullptr);

        const point_layout* vao = point_layout::get_bound();
        const shader* shader = shader::get_current_bound_for_debug();

        ASSERT(vao);
        ASSERT(shader);

        ASSERTX(get_bound() == this, get_bound(), this);

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

        if(mode == render_mode::triangles)
        {
            glDrawElements(GL_TRIANGLES, count * 3, GL_UNSIGNED_INT, nullptr);
        }
        else
        {
            glDrawElements(GL_LINES, count * 2, GL_UNSIGNED_INT, nullptr);
        }
    }

    void
    index_buffer::bind(const index_buffer* ebo)
    {
        const gluint id = ebo != nullptr ? ebo->id_ : 0;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        get_bound() = ebo;
    }

    const index_buffer*&
    index_buffer::get_bound()
    {
        static const index_buffer* ebo = nullptr;
        return ebo;
    }
}
