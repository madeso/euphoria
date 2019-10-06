#ifndef RENDER_INIT_H
#define RENDER_INIT_H

#include "core/rect.h"
#include "core/mat4.h"
#include "core/rgb.h"

namespace euphoria::render
{
    typedef void* (*LoaderFunction)(const char* name);

    // todo: come up with a better name
    class Init
    {
        public:
        enum class BlendHack
        {
            NoHack,
            EnableHack
        };

        explicit Init(
                LoaderFunction loader,
                BlendHack      blend_hack = BlendHack::NoHack);
        ~Init();

        core::mat4f
        GetOrthoProjection(float width, float height);

        // todo: move to some better place
        void
        Use2d();

        // todo: move to some place better
        void
        ClearScreen(const core::Rgb& color);

        bool ok;
    };

}  // namespace euphoria::render

#endif  // RENDER_INIT_H
