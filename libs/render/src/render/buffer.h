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
    struct VertexBuffer : public Id
    {
    public:
        VertexBuffer();
        ~VertexBuffer();

        NONCOPYABLE(VertexBuffer);

        void
        set_data(const std::vector<float>& data);

        static void
        bind(const VertexBuffer* vbo);

        static const VertexBuffer*&
        get_bound();
    };


    /** Stores what the data in the vertex_buffer is and how it is laid out/used
     * Represents a OpenGL Vertex Array Object (VAO).
     */
    struct PointLayout : public Id
    {
    public:
        PointLayout();
        ~PointLayout();

        NONCOPYABLE(PointLayout);

        void
        bind_data(const ShaderAttribute& attribute, int stride, int offset);

        static void
        bind(const PointLayout* vao);

        static const PointLayout*&
        get_bound();

        // debug
        std::vector<ShaderAttribute> attributes;
    };


    enum class RenderMode
    {
        lines,
        triangles
    };


    /** Reuses points.
     * Represents a OpenGL Element Buffer Object (EBO).
     */
    struct IndexBuffer : public Id
    {
    public:
        IndexBuffer();
        ~IndexBuffer();

        NONCOPYABLE(IndexBuffer);

        void
        set_data(const std::vector<unsigned int>& indices);

        // count = the number of triangles
        void
        draw(RenderMode mode, int count) const;

        static void
        bind(const IndexBuffer* ebo);

        static const IndexBuffer*&
        get_bound();
    };

}
