#pragma once

#include "core/mat4.h"
#include "core/vec3.h"
#include "core/quat.h"
#include "core/angle.h"

#include <optional>


namespace euphoria::window::imgui::guizmo
{
    void setup();
    void begin();

    bool
    transform
    (
        bool is_local,
        const std::optional<core::Vec3f>& snap,
        const core::Mat4f& camera_view,
        const core::Mat4f& camera_projection,
        const core::Mat4f& model,
        core::Vec3f* new_position
    );

    bool
    rotate
    (
        bool is_local,
        const std::optional<core::Angle>& snap,
        const core::Mat4f& camera_view,
        const core::Mat4f& camera_projection,
        const core::Mat4f& model,
        core::Quatf* new_rotation
    );
}
