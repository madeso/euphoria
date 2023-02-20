#pragma once

#include "core/rect.h"
#include "core/mat4.h"
#include "core/rgb.h"

namespace euphoria::render
{
    using LoaderFunction = void* (*)(const char* name);

    // todo(Gustav): come up with a better name
    struct Init
    {
        enum class BlendHack
        {
            no_hack,
            enable_hack
        };

        bool is_ok;

        explicit Init(LoaderFunction loader, BlendHack blend_hack = BlendHack::no_hack);
        ~Init();

        Init(const Init&) = delete;
        Init(Init&&) = delete;
        void operator=(const Init&) = delete;
        void operator=(Init&&) = delete;

        [[nodiscard]] core::mat4f get_ortho_projection
            (float width, float height) const;

        // todo(Gustav): move to some place better
        void clear_screen(const core::rgb& color) const;

        // todo(Gustav): move to some better place
        void use_2d() const;
    };

}
