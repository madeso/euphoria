#ifndef EUPHORIA_CANVASLOGIC_H
#define EUPHORIA_CANVASLOGIC_H

#include "core/vec2.h"
#include "core/range.h"

namespace euphoria::core
{
    class CanvasLogic
    {
        public:
        // canvas view
        vec2f scroll = vec2f {0, 0};
        float scale  = 1;

        // "config"
        Range<float> scale_range = MakeRange(0.1f, 15.0f);

        void
        Pan(const vec2f& p);

        void
        Zoom(const vec2f& mouse, float zoom);

        vec2f
        ScreenToWorld(const vec2f& p) const;

        vec2f
        WorldToScreen(const vec2f& p) const;
    };
}  // namespace euphoria::core

#endif  // EUPHORIA_CANVASLOGIC_H
