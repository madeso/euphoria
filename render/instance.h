#ifndef EUPHORIA_RENDER_INSTANCE_H
#define EUPHORIA_RENDER_INSTANCE_H

#include "core/mat4.h"
#include "core/vec3.h"

namespace euphoria::render
{
    struct Light;

    struct Instance
    {
        virtual ~Instance() = default;

        virtual void
        Render
        (
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera,
            const Light&       light
        ) = 0;
    };
}

#endif  // EUPHORIA_RENDER_INSTANCE_H
