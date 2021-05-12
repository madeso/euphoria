#include "gui/root.h"

#include "core/cint.h"

#include "render/spriterender.h"

#include "gui/load.h"
#include "gui/skin.h"


namespace euphoria::gui
{
    Root::Root(const core::Sizef& s)
        : size(s)
    {
    }


    Root::~Root() = default;


    bool
    Root::Load
    (
        core::vfs::file_system* fs,
        render::FontCache* font,
        const core::vfs::file_path& path,
        render::TextureCache* cache
    )
    {
        const bool result = euphoria::gui::Load(this, fs, font, path, cache);

        if(result)
        {
            container.DoLayout(core::rectf::from_width_height(size));
        }

        return result;
    }


    void
    Root::SetInputMouse(const core::vec2f& pos, bool down)
    {
        state.mouse = pos;
        state.mouse_down = down;
    }


    void
    Root::Step(float dt)
    {
        state.Begin();
        container.Step(dt);
        state.End();
    }


    void
    Root::Resize(const core::Sizef& new_size)
    {
        size = new_size;
        container.DoLayout(core::rectf::from_width_height(size));
    }


    void
    Root::Render(render::SpriteRenderer* sp) const
    {
        container.Render(sp);

        auto image = state.hot != nullptr ? hover_image : cursor_image;

        if(image)
        {
            sp->DrawSprite
            (
                *image,
                core::rectf::from_position_anchor_width_and_height
                (
                    state.mouse,
                    core::scale2f{0, 1},
                    core::Cint_to_float(image->width),
                    core::Cint_to_float(image->height)
                )
            );
        }
    }
}
