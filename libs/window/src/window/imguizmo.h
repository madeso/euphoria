#pragma once

#include "base/mat4.h"
#include "base/vec3.h"
#include "base/quat.h"
#include "base/angle.h"

#include <optional>


namespace eu::window::imgui::guizmo
{
    void setup();
    void begin();

    bool
    translate
    (
        bool is_local,
        const std::optional<vec3f>& snap,
        const mat4f& camera_view,
        const mat4f& camera_projection,
        const mat4f& model,
        bool x, bool y, bool z,
        vec3f* new_position
    );

    bool
    rotate
    (
        bool is_local,
        const std::optional<Angle>& snap,
        const mat4f& camera_view,
        const mat4f& camera_projection,
        const mat4f& model,
        bool y, bool p, bool r,
        quatf* new_rotation
    );
}
