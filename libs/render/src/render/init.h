#pragma once

#include "core/rect.h"
#include "core/mat4.h"
#include "core/rgb.h"
#include "core/noncopyable.h"

namespace euphoria::render
{
    using LoaderFunction = void* (*)(const char* name);

    // todo(Gustav): come up with a better name
    struct Init
    {
    public:
        enum class BlendHack
        {
            NoHack,
            EnableHack
        };

        explicit Init(LoaderFunction loader, BlendHack blend_hack = BlendHack::NoHack);
        ~Init();

        NONCOPYABLE(Init);

        [[nodiscard]] core::mat4f
        GetOrthoProjection(float width, float height) const;

        // todo(Gustav): move to some better place
        void
        Use2d() const;

        // todo(Gustav): move to some place better
        void
        ClearScreen(const core::rgb& color) const;

        bool ok;
    };

}
