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
            const mat4f& projection_matrix,
            const mat4f& view_matrix,
            const vec3f& camera,
            const render::Light& light
        ) override;

        std::shared_ptr<CompiledLines> lines;
    };
}
