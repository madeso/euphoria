#pragma once

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
        IndexBuffer lines;
        int line_count;
        std::shared_ptr<MaterialShader> shader;
        
        void
        render
        (
            const core::mat4f& model_matrix,
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix
        );
    };
    

    std::shared_ptr<CompiledLines>
    compile(MaterialShaderCache* shader_cache, const core::Lines& lines);
}


