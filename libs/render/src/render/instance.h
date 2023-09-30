#pragma once

#include "base/mat4.h"
#include "base/vec3.h"
#include "base/quat.h"


namespace eu::render
{
    struct Light;

    struct Instance
    {
        bool remove_this = false;
        vec3f position;
        quatf rotation;

        Instance();
        virtual ~Instance() = default;

        Instance(const Instance&) = delete;
        Instance(Instance&&) = delete;
        void operator=(const Instance&) = delete;
        void operator=(Instance&&) = delete;

        [[nodiscard]] mat4f calc_model_matrix() const;

        virtual void render
        (
            const mat4f& projection_matrix,
            const mat4f& view_matrix,
            const vec3f& camera,
            const Light& light
        ) = 0;
    };

    [[nodiscard]] mat4f calc_model_matrix
    (
        const vec3f& position,
        const quatf& rotation
    );
}
