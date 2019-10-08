#include "gui/root.h"

#include "render/spriterender.h"

#include "gui/load.h"
#include "gui/skin.h"

namespace euphoria::gui
{
    Root::Root(const core::Sizef& size) : size_(size) {}

    Root::~Root() {}

    bool
    Root::Load(
            core::vfs::FileSystem* fs,
            render::FontCache*     font,
            const std::string&     path,
            render::TextureCache*  cache)
    {
        const bool result = euphoria::gui::Load(this, fs, font, path, cache);
        if(result)
        {
            container_.DoLayout(core::Rectf::FromWidthHeight(size_));
        }
        return result;
    }

    void
    Root::SetInputMouse(const core::vec2f& pos, bool down)
    {
        state_.mouse      = pos;
        state_.mouse_down = down;
    }

    void
    Root::Step(float dt)
    {
        state_.Begin();
        container_.Step(dt);
        state_.End();
    }

    void
    Root::Render(render::SpriteRenderer* sp) const
    {
        container_.Render(sp);

        auto image = state_.hot != nullptr ? hover_image : cursor_image;

        if(image)
        {
            sp->DrawSprite(
                    *image,
                    core::Rectf::FromPositionAnchorWidthAndHeight(
                            state_.mouse,
                            core::scale2f {0, 1},
                            image->GetWidth(),
                            image->GetHeight()));
        }
    }
}  // namespace euphoria::gui
