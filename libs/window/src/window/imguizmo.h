#pragma once

#include "core/mat4.h"
#include "core/vec3.h"

#include <optional>


namespace euphoria::window::imgui::guizmo
{
    void setup();
    void begin();

    bool
    run
    (
        bool is_local,
        const std::optional<core::Vec3f>& snap,
        const core::Mat4f& camera_view,
        const core::Mat4f& camera_projection,
        const core::Mat4f& model,
        core::Vec3f* new_position
    );
}
