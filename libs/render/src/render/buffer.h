#pragma once

#include <vector>

#include "core/noncopyable.h"

#include "render/id.h"
#include "render/shaderattribute.h"

namespace euphoria::render
{
    /** Stores vertices, uv, etc.
     * Represents a OpenGL Vertex Buffer Object (VBO).
     */
    struct vertex_buffer : public id
    {
    public:
        vertex_buffer();
        ~vertex_buffer();

        NONCOPYABLE(vertex_buffer);

        void
        set_data(const std::vector<float>& data);

        static void
        bind(const vertex_buffer* vbo);

        static const vertex_buffer*&
        get_bound();
    };


    /** Stores what the data in the vertex_buffer is and how it is laid out/used
     * Represents a OpenGL Vertex Array Object (VAO).
     */
    struct point_layout : public id
    {
    public:
        point_layout();
        ~point_layout();

        NONCOPYABLE(point_layout);

        void
        bind_data(const shader_attribute& attribute, int stride, int offset);

        static void
        bind(const point_layout* vao);

        static const point_layout*&
        get_bound();

        // debug
        std::vector<shader_attribute> attributes;
    };


    enum class render_mode
    {
        lines,
        triangles
    };


    /** Reuses points.
     * Represents a OpenGL Element Buffer Object (EBO).
     */
    struct index_buffer : public id
    {
    public:
        index_buffer();
        ~index_buffer();

        NONCOPYABLE(index_buffer);

        void
        set_data(const std::vector<unsigned int>& indices);

        // count = the number of triangles
        void
        draw(render_mode mode, int count) const;

        static void
        bind(const index_buffer* ebo);

        static const index_buffer*&
        get_bound();
    };

}
