#include "render/positionedlines.h"

#include "assert/assert.h"

namespace eu::render
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
        const core::vec3f&,
        const Light&
    )
    {
        lines->render
        (
            calc_model_matrix(),
            projection_matrix,
            view_matrix
        );
    }
}
