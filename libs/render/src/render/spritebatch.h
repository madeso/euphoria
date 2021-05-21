#pragma once

#include <vector>

#include "core/rect.h"
#include "core/size2.h"
#include "core/vec2.h"
#include "core/angle.h"
#include "core/rgb.h"
#include "core/noncopyable.h"

namespace euphoria::render
{
    // texture & shader will be handled by 2d world and sprite
    struct sprite_batch
    {
    public:
        sprite_batch();
        ~sprite_batch();

        NONCOPYABLE(sprite_batch);

        void
        begin();

        void
        quad
        (
            const core::vec2f& pos,
            const core::size2f& quad,
            const core::rectf& uv,
            const core::scale2f& center,
            const core::angle& rotation,
            const core::rgba& color
        );

        void
        end();

        void
        flush();

    private:
        bool inside_;
        int count_;
        int rendercalls_;
        std::vector<float> data_;
        std::vector<int> index_;
    };

}
