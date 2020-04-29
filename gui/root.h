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
        explicit
        Root(const core::Sizef& s);

        ~Root();

        [[nodiscard]]
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

        std::vector<std::shared_ptr<Skin>> skins;
        core::Sizef size;
        UiState state;
        LayoutContainer container;
        std::shared_ptr<render::Texture2d> cursor_image;
        std::shared_ptr<render::Texture2d> hover_image;
    };
}

#endif  // GUI_ROOT_H

