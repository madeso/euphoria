#pragma once

#include "base/rect.h"
#include "base/size2.h"
#include "base/vec2.h"
#include "base/angle.h"
#include "base/rgb.h"


namespace eu::render
{
    // texture & shader will be handled by 2d world and sprite
    struct SpriteBatch
    {
    public:
        SpriteBatch();
        ~SpriteBatch();

        SpriteBatch(const SpriteBatch& other) = delete;
        void operator=(const SpriteBatch&) = delete;
        SpriteBatch(SpriteBatch&& other) = delete;
        void operator=(SpriteBatch&&) = delete;

        void
        begin();

        void
        render_quad
        (
            const vec2f& pos,
            const size2f& quad,
            const Rectf& uv,
            const Scale2f& center,
            const Angle& rotation,
            const Rgba& color
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
