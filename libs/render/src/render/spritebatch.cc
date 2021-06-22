#include "render/spritebatch.h"
#include "render/gl.h"

#include "core/assert.h"
#include "core/cint.h"

namespace euphoria::render
{
    // vertex + uv + color
    constexpr int quad_cont = 10;
    constexpr int stride = 2 * 4 + 2 * 4 + 4 * 4;


    sprite_batch::sprite_batch() : is_inside(false), current_quad_count(0), number_of_render_calls(0)
    {
        vertex_data.reserve(core::c_int_to_sizet(stride * quad_cont));
        quad_indices.reserve(core::c_int_to_sizet(6 * quad_cont));
    }


    sprite_batch::~sprite_batch() = default;


    void
    sprite_batch::begin()
    {
        ASSERT(!is_inside && "Already open, missing call to end.");
        number_of_render_calls = 0;
    }


    void
    sprite_batch::quad
    (
        const core::vec2f& pos,
        const core::size2f& quad,
        const core::rectf& uv,
        const core::scale2f& center,
        const core::angle& rotation,
        const core::rgba& color
    )
    {
        ASSERT(is_inside && "batch need to be open");

        if((current_quad_count + 1) >= quad_cont)
        {
            flush();
        }

        // add vertices
        const float w = quad.width;
        const float h = quad.height;
        const float sy = -sin(rotation);
        const float left = pos.x + (-center.x * w) * sy;
        const float right = pos.x + (-center.x * w + w) * sy;
        const float top = pos.y + (-center.y * h) * sy;
        const float bottom = pos.y + (-center.y * h + h) * sy;
        const auto upper_left = core::vec2f(left, top);
        const auto upper_right = core::vec2f(right, top);
        const auto lower_left = core::vec2f(left, bottom);
        const auto lower_right = core::vec2f(right, bottom);

        vertex_data.push_back(upper_left.x);
        vertex_data.push_back(upper_left.y);

        vertex_data.push_back(upper_right.x);
        vertex_data.push_back(upper_right.y);

        vertex_data.push_back(lower_right.x);
        vertex_data.push_back(lower_right.y);

        vertex_data.push_back(lower_left.x);
        vertex_data.push_back(lower_left.y);

        // add uv coordinate
        vertex_data.push_back(uv.left);
        vertex_data.push_back(uv.top);
        vertex_data.push_back(uv.right);
        vertex_data.push_back(uv.top);
        vertex_data.push_back(uv.right);
        vertex_data.push_back(uv.bottom);
        vertex_data.push_back(uv.left);
        vertex_data.push_back(uv.bottom);

        // add color
        for(int i = 0; i < 4; ++i)
        {
            vertex_data.push_back(color.r);
            vertex_data.push_back(color.g);
            vertex_data.push_back(color.b);
            vertex_data.push_back(color.a);
        }

        // add index
        int start = current_quad_count * 4;

        quad_indices.push_back(start + 0);
        quad_indices.push_back(start + 1);
        quad_indices.push_back(start + 2);

        quad_indices.push_back(start + 0);
        quad_indices.push_back(start + 2);
        quad_indices.push_back(start + 3);

        current_quad_count += 1;
    }


    void
    sprite_batch::end()
    {
        ASSERT(is_inside && "not open, missing begin.");
        flush();
        is_inside = false;
    }


    void
    sprite_batch::flush()
    {
        if(current_quad_count == 0)
        {
            return;
        }
        // todo(Gustav): build vbo & render vbo
        // remove all items
        current_quad_count = 0;
        vertex_data.resize(0);
        number_of_render_calls += 1;
    }
}
