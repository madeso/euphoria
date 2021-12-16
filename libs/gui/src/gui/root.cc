#include "gui/root.h"

#include "core/cint.h"

#include "render/spriterender.h"

#include "gui/load.h"
#include "gui/skin.h"


namespace euphoria::gui
{
    Root::Root(const core::Size2f& s)
        : size(s)
    {
    }


    Root::~Root() = default;


    bool
    Root::load
    (
        core::vfs::FileSystem* fs,
        render::FontCache* font,
        const core::vfs::FilePath& path,
        render::TextureCache* cache
    )
    {
        const bool result = euphoria::gui::load_gui(this, fs, font, path, cache);

        if(result)
        {
            container.do_layout(core::Rectf::from_width_height(size));
        }

        return result;
    }


    void
    Root::set_input_mouse(const core::Vec2f& pos, bool down)
    {
        state.mouse = pos;
        state.mouse_down = down;
    }


    void
    Root::step(float dt)
    {
        state.begin();
        container.step(dt);
        state.end();
    }


    void
    Root::resize(const core::Size2f& new_size)
    {
        size = new_size;
        container.do_layout(core::Rectf::from_width_height(size));
    }


    void
    Root::render(render::SpriteRenderer* sp) const
    {
        container.render(sp);

        auto image = state.hot != nullptr ? hover_image : cursor_image;

        if(image)
        {
            sp->draw_sprite
            (
                *image,
                core::Rectf::from_position_anchor_width_and_height
                (
                    state.mouse,
                    core::Scale2f{0, 1},
                    core::c_int_to_float(image->width),
                    core::c_int_to_float(image->height)
                )
            );
        }
    }
}
