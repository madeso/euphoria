#include "core/canvaslogic.h"
#include "core/numeric.h"

namespace euphoria::core
{
    void
    CanvasLogic::pan(const vec2f& p)
    {
        scroll += p;
    }

    void
    CanvasLogic::zoom(const vec2f& mouse, float zoom)
    {
        // todo(Gustav): change to use screen_to_world
        const auto focus = (mouse - scroll) / scale;

        const float scale_factor = 1 + 0.01f * abs(zoom);

        if(zoom < 0.0f)
        {
            scale /= scale_factor;
        }

        if(zoom > 0.0f)
        {
            scale *= scale_factor;
        }

        scale = keep_within(scale_range, scale);

        // todo(Gustav): change to use world_to_screen
        const auto new_focus = scroll + focus * scale;
        scroll = scroll + vec2f::from_to(new_focus, mouse);
    }

    vec2f
    CanvasLogic::from_world_to_screen(const vec2f& p) const
    {
        return scroll + p * scale;
    }

    vec2f
    CanvasLogic::from_screen_to_world(const vec2f& p) const
    {
        return (p - scroll) / scale;
    }
}
