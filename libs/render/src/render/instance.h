#pragma once

#include "core/mat4.h"
#include "core/vec3.h"
#include "core/quat.h"


namespace eu::render
{
    struct Light;

    struct Instance
    {
        bool remove_this = false;
        core::vec3f position;
        core::quatf rotation;

        Instance();
        virtual ~Instance() = default;

        Instance(const Instance&) = delete;
        Instance(Instance&&) = delete;
        void operator=(const Instance&) = delete;
        void operator=(Instance&&) = delete;

        [[nodiscard]] core::mat4f calc_model_matrix() const;

        virtual void render
        (
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera,
            const Light& light
        ) = 0;
    };

    [[nodiscard]] core::mat4f calc_model_matrix
    (
        const core::vec3f& position,
        const core::quatf& rotation
    );
}
