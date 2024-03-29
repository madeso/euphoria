#pragma once

#include "base/vec3.h"
#include "base/mat4.h"

#include "render/buffer.h"

#include <memory>

namespace eu::core
{
    struct Lines;
}

namespace eu::render
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
            const mat4f& model_matrix,
            const mat4f& projection_matrix,
            const mat4f& view_matrix
        );
    };
    

    std::shared_ptr<CompiledLines>
    compile(MaterialShaderCache* shader_cache, const core::Lines& lines);
}


