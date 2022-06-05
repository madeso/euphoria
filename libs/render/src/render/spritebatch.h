#pragma once



#include "core/rect.h"
#include "core/size2.h"
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
        begin();

        void
        quad
        (
            const core::Vec2f& pos,
            const core::Size2f& quad,
            const core::Rectf& uv,
            const core::Scale2f& center,
            const core::Angle& rotation,
            const core::Rgba& color
        );

        void
        end();

        void
        flush();

    private:
        bool is_inside;
        int current_quad_count;
        int number_of_render_calls;
        std::vector<float> vertex_data;
        std::vector<int> quad_indices;
    };

}
