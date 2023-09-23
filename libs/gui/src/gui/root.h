#pragma once

#include "core/vec2.h"

#include "gui/uistate.h"
#include "gui/layoutcontainer.h"



#include <memory>


namespace eu::io
{
    struct FileSystem;
    struct FilePath;
}

namespace eu::render
{
    struct Texture2;
    struct FontCache;
    struct TextureCache;
}

namespace eu::gui
{
    struct Skin;
}


namespace eu::gui
{

    struct Root
    {
        explicit Root(const core::size2f& s);
        ~Root();

        Root(const Root& other) = delete;
        void operator=(const Root&) = delete;
        Root(Root&& other) = delete;
        void operator=(Root&&) = delete;

        [[nodiscard]] bool
        load
        (
            io::FileSystem* fs,
            render::FontCache* font,
            const ::eu::io::FilePath& path,
            render::TextureCache* cache
        );

        void
        set_input_mouse(const core::vec2f& pos, bool down);

        void
        update(float dt);

        void
        set_size(const core::size2f& new_size);

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

