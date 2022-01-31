#include "render/positionedlines.h"

#include "assert/assert.h"

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
        const core::Mat4f& projection_matrix,
        const core::Mat4f& view_matrix,
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
