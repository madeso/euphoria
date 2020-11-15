#ifndef EUPHORIA_RENDER_POSITIONEDLINES_H
#define EUPHORIA_RENDER_POSITIONEDLINES_H

#include <memory>

#include "render/instance.h"
#include "render/compiledlines.h"

namespace euphoria::render
{
    struct PositionedLines : public Instance
    {
        PositionedLines(const std::shared_ptr<CompiledLines>& alines);

        void
        Render
        (
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera,
            const Light&       light
        ) override;

        std::shared_ptr<CompiledLines> lines;
    };
}

#endif  // EUPHORIA_RENDER_POSITIONEDLINES_H

