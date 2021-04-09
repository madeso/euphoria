#ifndef EUPHORIA_RENDER_COMPILEDLINES_H
#define EUPHORIA_RENDER_COMPILEDLINES_H

#include "core/vec3.h"
#include "core/mat4.h"

#include "render/buffer.h"

#include <memory>

namespace euphoria::core
{
    struct Lines;
}

namespace euphoria::render
{
    struct MaterialShader;
    struct MaterialShaderCache;
    
    struct CompiledLines
    {
        VertexBuffer data;
        PointLayout config;
        IndexBuffer  lines;
        int          line_count;
        std::shared_ptr<MaterialShader> shader;
        
        void
        Render
        (
            const core::mat4f& model_matrix,
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera
        );
    };
    

    std::shared_ptr<CompiledLines>
    Compile(MaterialShaderCache* shader_cache, const core::Lines& lines);
}

#endif  // EUPHORIA_RENDER_COMPILEDLINES_H
