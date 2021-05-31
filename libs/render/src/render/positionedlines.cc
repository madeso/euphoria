#include "render/positionedlines.h"

#include "core/assert.h"

namespace euphoria::render
{
    positioned_lines::positioned_lines
    (
        const std::shared_ptr<compiled_lines>& alines
    )
        : lines(alines)
    {
        ASSERT(alines);
    }

    void
    positioned_lines::render
    (
        const core::mat4f& projection_matrix,
        const core::mat4f& view_matrix,
        const core::vec3f& camera,
        const light&
    )
    {
        lines->render
        (
            calculate_model_matrix(),
            projection_matrix,
            view_matrix,
            camera
        );
    }
}
