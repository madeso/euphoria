#ifndef GUI_ROOT_H
#define GUI_ROOT_H

#include <string>
#include <vector>
#include <memory>

#include "core/vec2.h"

#include "gui/uistate.h"
#include "gui/layoutcontainer.h"


namespace euphoria::core::vfs
{
    struct FileSystem;
    struct FilePath;
}

namespace euphoria::render
{
    struct Texture2d;
    struct FontCache;
    struct TextureCache;
}

namespace euphoria::gui
{
    struct Skin;

    struct Root
    {
    public:
        Root(const core::Sizef& size);
        ~Root();

        bool
        Load
        (
            core::vfs::FileSystem* fs,
            render::FontCache* font,
            const ::euphoria::core::vfs::FilePath& path,
            render::TextureCache* cache
        );

        void
        SetInputMouse(const core::vec2f& pos, bool down);

        void
        Step(float dt);

        void
        Render(render::SpriteRenderer* sp) const;

        std::vector<std::shared_ptr<Skin>> skins_;
        core::Sizef size_;
        UiState state_;
        LayoutContainer container_;
        std::shared_ptr<render::Texture2d> cursor_image;
        std::shared_ptr<render::Texture2d> hover_image;
    };
}

#endif  // GUI_ROOT_H
