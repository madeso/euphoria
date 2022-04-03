#pragma once

#include "core/rect.h"
#include "core/vec2.h"
#include "core/ray.h"

namespace euphoria::core
{
    struct Viewport
    {
        Viewport(const Recti& r);

        [[nodiscard]] float
        get_aspect_ratio() const;

        [[nodiscard]] Vec2f
        to_clip_coord(const Vec2i& p) const;

        Recti bounds;
    };

    struct CompiledCamera3;

    Ray3f mouse_to_ray
    (
        const CompiledCamera3& camera,
        const Viewport& viewport,
        const Vec2i& position
    );

    UnitRay3f mouse_to_unit_ray
    (
        const CompiledCamera3& camera,
        const Viewport& viewport,
        const Vec2i& position
    );
}
