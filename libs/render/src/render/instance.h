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

        [[nodiscard]] core::Mat4f
        calculate_model_matrix() const;

        virtual void
        render
        (
            const core::Mat4f& projection_matrix,
            const core::Mat4f& view_matrix,
            const core::Vec3f& camera,
            const Light& light
        ) = 0;

        bool remove_this = false;

        core::Vec3f position;
        core::Quatf rotation;
    };

    [[nodiscard]] core::Mat4f
    calculate_model_matrix
    (
        const core::Vec3f& position,
        const core::Quatf& rotation
    );
}
