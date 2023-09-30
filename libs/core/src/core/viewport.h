#pragma once

#include "core/rect.h"
#include "base/vec2.h"
#include "core/ray.h"

namespace eu::core
{
    struct Viewport
    {
        Viewport(const Recti& r);

        [[nodiscard]] float
        get_aspect_ratio() const;

        [[nodiscard]] vec2f
        to_clip_coord(const vec2i& p) const;

        Recti bounds;
    };

    struct CompiledCamera3;

    Ray3f from_mouse_to_ray
    (
        const CompiledCamera3& camera,
        const Viewport& viewport,
        const vec2i& position
    );

    UnitRay3f from_mouse_to_unit_ray
    (
        const CompiledCamera3& camera,
        const Viewport& viewport,
        const vec2i& position
    );
}
