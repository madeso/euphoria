#pragma once

#include <memory>



namespace eu::render
{
    struct DrawableFont;
    struct DrawableText;
}

namespace eu::gui
{
    struct TextData
    {
        TextData();
        ~TextData();

        TextData(const TextData& other) = delete;
        void operator=(const TextData&) = delete;
        TextData(TextData&& other) = delete;
        void operator=(TextData&&) = delete;

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
