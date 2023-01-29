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
    translate
    (
        bool is_local,
        const std::optional<core::vec3f>& snap,
        const core::mat4f& camera_view,
        const core::mat4f& camera_projection,
        const core::mat4f& model,
        bool x, bool y, bool z,
        core::vec3f* new_position
    );

    bool
    rotate
    (
        bool is_local,
        const std::optional<core::angle>& snap,
        const core::mat4f& camera_view,
        const core::mat4f& camera_projection,
        const core::mat4f& model,
        bool y, bool p, bool r,
        core::quatf* new_rotation
    );
}
