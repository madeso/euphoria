#pragma once

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
    struct font_cache;
    struct texture_cache;
}

namespace euphoria::gui
{
    struct skin;
}


namespace euphoria::gui
{

    struct root
    {
        explicit root(const core::size2f& s);
        ~root();

        NONCOPYABLE(root);

        [[nodiscard]] bool
        load
        (
            core::vfs::FileSystem* fs,
            render::font_cache* font,
            const ::euphoria::core::vfs::FilePath& path,
            render::texture_cache* cache
        );

        void
        set_input_mouse(const core::Vec2f& pos, bool down);

        void
        step(float dt);

        void
        resize(const core::size2f& new_size);

        void
        render(render::sprite_renderer* sp) const;

        std::vector<std::shared_ptr<skin>> skins;
        core::size2f size;
        ui_state state;
        layout_container container;
        std::shared_ptr<render::texture2d> cursor_image;
        std::shared_ptr<render::texture2d> hover_image;
    };
}

