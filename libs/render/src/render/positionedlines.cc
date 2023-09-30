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
        const mat4f& projection_matrix,
        const mat4f& view_matrix,
        const vec3f&,
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
