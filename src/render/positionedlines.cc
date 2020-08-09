#include "render/positionedlines.h"

#include "core/assert.h"

namespace euphoria::render
{
    PositionedLines::PositionedLines
    (
        const std::shared_ptr<CompiledLines>& alines
    )
        : lines(alines)
    {
        ASSERT(alines);
    }

    void
    PositionedLines::Render
    (
        const core::mat4f& projection_matrix,
        const core::mat4f& view_matrix,
        const core::vec3f& camera,
        const Light&
    )
    {
        lines->Render
        (
            GetModelMatrix(),
            projection_matrix,
            view_matrix,
            camera
        );
    }
}
