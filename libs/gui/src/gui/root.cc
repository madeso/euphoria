#include "gui/root.h"

#include "base/cint.h"

#include "render/spriterender.h"

#include "gui/load.h"
#include "gui/skin.h"


namespace eu::gui
{
    Root::Root(const size2f& s)
        : size(s)
    {
    }


    Root::~Root() = default;


    bool
    Root::load
    (
        io::FileSystem* fs,
        render::FontCache* font,
        const io::FilePath& path,
        render::TextureCache* cache
    )
    {
        const bool result = eu::gui::load_gui(this, fs, font, path, cache);

        if(result)
        {
            container.lay_out(Rectf::from_width_height(size));
        }

        return result;
    }


    void
    Root::set_input_mouse(const vec2f& pos, bool down)
    {
        state.mouse = pos;
        state.mouse_down = down;
    }


    void
    Root::update(float dt)
    {
        state.begin();
        container.update(dt);
        state.end();
    }


    void
    Root::set_size(const size2f& new_size)
    {
        size = new_size;
        container.lay_out(Rectf::from_width_height(size));
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
                Rectf::from_position_anchor_width_and_height
                (
                    state.mouse,
                    Scale2f{0, 1},
                    c_int_to_float(image->width),
                    c_int_to_float(image->height)
                )
            );
        }
    }
}
