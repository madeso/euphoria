#pragma once

#include "core/noncopyable.h"

#include <memory>
#include <string>


namespace euphoria::render
{
    struct DrawableFont;
    struct DrawableText;
}

namespace euphoria::gui
{
    struct TextData
    {
        TextData();
        ~TextData();

        NONCOPYABLE(TextData);

        void
        set_font(std::shared_ptr<render::DrawableFont> font);

        [[nodiscard]] const render::DrawableFont&
        get_font() const;

        void
        update_string(const std::string& str);

        [[nodiscard]] bool
        has_text() const;

        [[nodiscard]] const render::DrawableText&
        get_text() const;

        render::DrawableText&
        get_text();

        void
        set_size(float size);

        void
        update_text();


        std::shared_ptr<render::DrawableFont> font;
        std::string string;
        float size;
        std::shared_ptr<render::DrawableText> text;
    };
}
