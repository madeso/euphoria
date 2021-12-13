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
    struct Texture2;
    struct FontCache;
    struct TextureCache;
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
            render::FontCache* font,
            const ::euphoria::core::vfs::FilePath& path,
            render::TextureCache* cache
        );

        void
        set_input_mouse(const core::Vec2f& pos, bool down);

        void
        step(float dt);

        void
        resize(const core::size2f& new_size);

        void
        render(render::SpriteRenderer* sp) const;

        std::vector<std::shared_ptr<skin>> skins;
        core::size2f size;
        ui_state state;
        layout_container container;
        std::shared_ptr<render::Texture2> cursor_image;
        std::shared_ptr<render::Texture2> hover_image;
    };
}

