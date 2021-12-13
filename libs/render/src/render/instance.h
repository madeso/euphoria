#pragma once

#include "core/mat4.h"
#include "core/vec3.h"
#include "core/quat.h"
#include "core/noncopyable.h"


namespace euphoria::render
{
    struct Light;

    struct Instance
    {
        Instance();
        virtual ~Instance() = default;

        NONCOPYABLE(Instance);

        [[nodiscard]] core::mat4f
        calculate_model_matrix() const;

        virtual void
        render
        (
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::Vec3f& camera,
            const Light& light
        ) = 0;

        bool remove_this = false;

        core::Vec3f position;
        core::Quatf rotation;
    };
}
