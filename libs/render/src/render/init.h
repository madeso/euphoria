#pragma once

#include "core/rect.h"
#include "core/mat4.h"
#include "core/rgb.h"
#include "core/noncopyable.h"

namespace euphoria::render
{
    using LoaderFunction = void* (*)(const char* name);

    // todo(Gustav): come up with a better name
    struct init
    {
    public:
        enum class blend_hack
        {
            no_hack,
            enable_hack
        };

        explicit init(LoaderFunction loader, blend_hack blend_hack = blend_hack::no_hack);
        ~init();

        NONCOPYABLE(init);

        [[nodiscard]] core::mat4f
        get_ortho_projection(float width, float height) const;

        // todo(Gustav): move to some better place
        void
        use_2d() const;

        // todo(Gustav): move to some place better
        void
        clear_screen(const core::rgb& color) const;

        bool is_ok;
    };

}
