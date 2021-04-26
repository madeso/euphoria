#pragma once

#include <vector>

#include "core/rect.h"
#include "core/size.h"
#include "core/vec2.h"
#include "core/angle.h"
#include "core/rgb.h"
#include "core/noncopyable.h"

namespace euphoria::render
{
    // texture & shader will be handled by 2d world and sprite
    struct SpriteBatch
    {
    public:
        SpriteBatch();
        ~SpriteBatch();

        NONCOPYABLE(SpriteBatch);

        void
        Begin();

        void
        Quad
        (
            const core::vec2f& pos,
            const core::Sizef& quad,
            const core::rectf& uv,
            const core::scale2f& center,
            const core::angle& rotation,
            const core::rgba& color
        );

        void
        End();

        void
        Flush();

    private:
        bool inside_;
        int count_;
        int rendercalls_;
        std::vector<float> data_;
        std::vector<int> index_;
    };

}
