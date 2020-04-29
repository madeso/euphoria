#include "gui/root.h"

#include "render/spriterender.h"

#include "gui/load.h"
#include "gui/skin.h"


namespace euphoria::gui
{
    Root::Root(const core::Sizef& s)
        : size(s)
    {
    }


    Root::~Root()
    {
    }


    bool
    Root::Load
    (
        core::vfs::FileSystem* fs,
        render::FontCache* font,
        const core::vfs::FilePath& path,
        render::TextureCache* cache
    )
    {
        const bool result = euphoria::gui::Load(this, fs, font, path, cache);

        if(result)
        {
            container.DoLayout(core::Rectf::FromWidthHeight(size));
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
        container.DoLayout(core::Rectf::FromWidthHeight(size));
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
                core::Rectf::FromPositionAnchorWidthAndHeight
                (
                    state.mouse,
                    core::scale2f{0, 1},
                    image->GetWidth(),
                    image->GetHeight()
                )
            );
        }
    }
}

