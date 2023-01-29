#pragma once

#include "core/vec2.h"
#include "core/noncopyable.h"

#include "gui/uistate.h"
#include "gui/layoutcontainer.h"



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
    struct Skin;
}


namespace euphoria::gui
{

    struct Root
    {
        explicit Root(const core::size2f& s);
        ~Root();

        NONCOPYABLE(Root);

        [[nodiscard]] bool
        load
        (
            core::vfs::FileSystem* fs,
            render::FontCache* font,
            const ::euphoria::core::vfs::FilePath& path,
            render::TextureCache* cache
        );

        void
        set_input_mouse(const core::vec2f& pos, bool down);

        void
        step(float dt);

        void
        resize(const core::size2f& new_size);

        void
        render(render::SpriteRenderer* sp) const;

        std::vector<std::shared_ptr<Skin>> skins;
        core::size2f size;
        State state;
        LayoutContainer container;
        std::shared_ptr<render::Texture2> cursor_image;
        std::shared_ptr<render::Texture2> hover_image;
    };
}

