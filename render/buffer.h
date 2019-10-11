#ifndef RENDER_BUFFER_H
#define RENDER_BUFFER_H

#include <vector>

#include "render/id.h"
#include "render/shaderattribute.h"

namespace euphoria::render
{
    // Vertex Buffer Object
    // Stores vertices, uv, etc
    struct Vbo : public Id
    {
    public:
        Vbo();
        ~Vbo();

        void
        SetData(const std::vector<float>& data);

        static void
        Bind(const Vbo* vbo);
        static const Vbo*&
        GetBound();
    };

    // Vertex Array Object
    // Stores what the data in the Vbo is and how it is laid out/used
    struct Vao : public Id
    {
    public:
        Vao();
        ~Vao();

        void
        BindVboData(const ShaderAttribute& attribute, int stride, int offset);

        static void
        Bind(const Vao* vao);
        static const Vao*&
        GetBound();

        // debug
        std::vector<ShaderAttribute> attributes;
    };

    // Element Buffer Object
    // Reuses points
    struct Ebo : public Id
    {
    public:
        Ebo();
        ~Ebo();

        void
        SetData(const std::vector<unsigned int>& indices);

        // count = the number of triangles
        void
        Draw(int count) const;

        static void
        Bind(const Ebo* ebo);

        static const Ebo*&
        GetBound();
    };

}  // namespace euphoria::render

#endif  // RENDER_BUFFER_H
