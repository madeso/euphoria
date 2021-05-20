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
    struct file_system;
    struct file_path;
}

namespace euphoria::render
{
    struct texture2d;
    struct font_cache;
    struct texture_cache;
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
            core::vfs::file_system* fs,
            render::font_cache* font,
            const ::euphoria::core::vfs::file_path& path,
            render::texture_cache* cache
        );

        void
        SetInputMouse(const core::vec2f& pos, bool down);

        void
        Step(float dt);

        void
        Resize(const core::Sizef& new_size);

        void
        Render(render::sprite_renderer* sp) const;

        std::vector<std::shared_ptr<Skin>> skins;
        core::Sizef size;
        UiState state;
        LayoutContainer container;
        std::shared_ptr<render::texture2d> cursor_image;
        std::shared_ptr<render::texture2d> hover_image;
    };
}

#endif  // GUI_ROOT_H

