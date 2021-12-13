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
    PositionedLines::render
    (
        const core::mat4f& projection_matrix,
        const core::mat4f& view_matrix,
        const core::Vec3f&,
        const Light&
    )
    {
        lines->render
        (
            calculate_model_matrix(),
            projection_matrix,
            view_matrix
        );
    }
}
