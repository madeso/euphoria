#pragma once

#include <memory>

#include "render/instance.h"
#include "render/compiledlines.h"

namespace euphoria::render
{
    struct positioned_lines : public instance
    {
        positioned_lines(const std::shared_ptr<compiled_lines>& alines);

        void
        render
        (
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera,
            const render::light& light
        ) override;

        std::shared_ptr<compiled_lines> lines;
    };
}
