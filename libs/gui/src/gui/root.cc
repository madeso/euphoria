#include "gui/root.h"

#include "core/cint.h"

#include "render/spriterender.h"

#include "gui/load.h"
#include "gui/skin.h"


namespace euphoria::gui
{
    root::root(const core::size2f& s)
        : size(s)
    {
    }


    root::~root() = default;


    bool
    root::load
    (
        core::vfs::file_system* fs,
        render::font_cache* font,
        const core::vfs::file_path& path,
        render::texture_cache* cache
    )
    {
        const bool result = euphoria::gui::load_gui(this, fs, font, path, cache);

        if(result)
        {
            container.do_layout(core::rectf::from_width_height(size));
        }

        return result;
    }


    void
    root::set_input_mouse(const core::vec2f& pos, bool down)
    {
        state.mouse = pos;
        state.mouse_down = down;
    }


    void
    root::step(float dt)
    {
        state.begin();
        container.step(dt);
        state.end();
    }


    void
    root::resize(const core::size2f& new_size)
    {
        size = new_size;
        container.do_layout(core::rectf::from_width_height(size));
    }


    void
    root::render(render::sprite_renderer* sp) const
    {
        container.render(sp);

        auto image = state.hot != nullptr ? hover_image : cursor_image;

        if(image)
        {
            sp->draw_sprite
            (
                *image,
                core::rectf::from_position_anchor_width_and_height
                (
                    state.mouse,
                    core::scale2f{0, 1},
                    core::c_int_to_float(image->width),
                    core::c_int_to_float(image->height)
                )
            );
        }
    }
}
