#ifndef RENDER_BUFFER_H
#define RENDER_BUFFER_H

#include <vector>

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

        void
        SetData(const std::vector<float>& data);

        static void
        Bind(const VertexBuffer* vbo);

        static const VertexBuffer*&
        GetBound();
    };

    /** Stores what the data in the VertexBuffer is and how it is laid out/used
     * Represents a OpenGL Vertex Array Object (VAO).
     */
    struct PointLayout : public Id
    {
    public:
        PointLayout();
        ~PointLayout();

        void
        BindData(const ShaderAttribute& attribute, int stride, int offset);

        static void
        Bind(const PointLayout* vao);

        static const PointLayout*&
        GetBound();

        // debug
        std::vector<ShaderAttribute> attributes;
    };

    /** Reuses points.
     * Represents a OpenGL Element Buffer Object (EBO).
     */
    struct IndexBuffer : public Id
    {
    public:
        IndexBuffer();
        ~IndexBuffer();

        void
        SetData(const std::vector<unsigned int>& indices);

        // count = the number of triangles
        void
        Draw(int count) const;

        static void
        Bind(const IndexBuffer* ebo);

        static const IndexBuffer*&
        GetBound();
    };

}  // namespace euphoria::render

#endif  // RENDER_BUFFER_H
