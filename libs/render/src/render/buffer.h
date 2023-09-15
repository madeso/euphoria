#pragma once

#include "render/id.h"
#include "render/shaderattribute.h"


namespace eu::render
{
    /** Stores vertices, uv, etc.
     * Represents a OpenGL Vertex Buffer Object (VBO).
     */
    struct VertexBuffer : public Id
    {
        VertexBuffer();
        ~VertexBuffer();

        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer(VertexBuffer&&) = delete;
        void operator=(const VertexBuffer&) = delete;
        void operator=(VertexBuffer&&) = delete;

        void set_data(const std::vector<float>& data);

        static void bind(const VertexBuffer* vbo);

        static const VertexBuffer*& get_bound();
    };


    /** Stores what the data in the vertex_buffer is and how it is laid out/used
     * Represents a OpenGL Vertex Array Object (VAO).
     */
    struct PointLayout : public Id
    {
        std::vector<ShaderAttribute> attributes;

        PointLayout();
        ~PointLayout();

        PointLayout(const PointLayout&) = delete;
        PointLayout(PointLayout&&) = delete;
        void operator=(const PointLayout&) = delete;
        void operator=(PointLayout&&) = delete;

        static const PointLayout*& get_bound();

        void bind_data(const ShaderAttribute& attribute, int stride, int offset);

        static void bind(const PointLayout* vao);
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
        IndexBuffer();
        ~IndexBuffer();

        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer(IndexBuffer&&) = delete;
        void operator=(const IndexBuffer&) = delete;
        void operator=(IndexBuffer&&) = delete;

        static const IndexBuffer*& get_bound();

        void set_data(const std::vector<unsigned int>& indices);

        /// @param count the number of triangles
        void draw(RenderMode mode, int count) const;

        static void bind(const IndexBuffer* ebo);
    };

}
