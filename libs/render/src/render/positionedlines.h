#pragma once

#include <memory>

#include "render/instance.h"
#include "render/compiledlines.h"

namespace eu::render
{
    struct PositionedLines : public Instance
    {
        PositionedLines(const std::shared_ptr<CompiledLines>& alines);

        void
        render
        (
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera,
            const render::Light& light
        ) override;

        std::shared_ptr<CompiledLines> lines;
    };
}
