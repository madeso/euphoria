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
    struct material_shader;
    struct material_shader_cache;
    
    struct compiled_lines
    {
        vertex_buffer data;
        point_layout config;
        index_buffer lines;
        int line_count;
        std::shared_ptr<material_shader> shader;
        
        void
        render
        (
            const core::mat4f& model_matrix,
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix
        );
    };
    

    std::shared_ptr<compiled_lines>
    compile(material_shader_cache* shader_cache, const core::Lines& lines);
}


