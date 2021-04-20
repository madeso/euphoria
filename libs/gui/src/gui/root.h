#ifndef GUI_ROOT_H
#define GUI_ROOT_H

#include "core/vec2.h"
#include "core/noncopyable.h"

#include "gui/uistate.h"
#include "gui/layoutcontainer.h"

#include <string>
#include <vector>
#include <memory>


namespace euphoria::core::vfs
{
    struct FileSystem;
    struct FilePath;
}

namespace euphoria::render
{
    struct texture2d;
    struct FontCache;
    struct TextureCache;
}

namespace euphoria::gui
{
    struct Skin;
}


namespace euphoria::gui
{

    struct Root
    {
        explicit Root(const core::Sizef& s);
        ~Root();

        NONCOPYABLE(Root);

        [[nodiscard]] bool
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
        Resize(const core::Sizef& new_size);

        void
        Render(render::SpriteRenderer* sp) const;

        std::vector<std::shared_ptr<Skin>> skins;
        core::Sizef size;
        UiState state;
        LayoutContainer container;
        std::shared_ptr<render::texture2d> cursor_image;
        std::shared_ptr<render::texture2d> hover_image;
    };
}

#endif  // GUI_ROOT_H

