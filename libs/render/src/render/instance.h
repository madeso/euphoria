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


        // todo(Gustav): remove geters and setters

        [[nodiscard]] const core::vec3f&
        GetPosition() const;

        [[nodiscard]] const core::quatf&
        GetRotation() const;

        void
        SetPosition(const core::vec3f& position);

        void
        SetRotation(const core::quatf& rotation);

        [[nodiscard]] core::mat4f
        GetModelMatrix() const;

        virtual void
        Render
        (
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera,
            const Light& light
        ) = 0;

        bool remove_this = false;

        core::vec3f position;
        core::quatf rotation;
    };
}
