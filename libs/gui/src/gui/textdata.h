#pragma once

#include "core/noncopyable.h"

#include <memory>
#include <string>


namespace euphoria::render
{
    struct drawable_font;
    struct drawable_text;
}

namespace euphoria::gui
{
    struct text_data
    {
        text_data();
        ~text_data();

        NONCOPYABLE(text_data);

        void
        set_font(std::shared_ptr<render::drawable_font> font);

        [[nodiscard]] const render::drawable_font&
        get_font() const;

        void
        update_string(const std::string& str);

        [[nodiscard]] bool
        has_text() const;

        [[nodiscard]] const render::drawable_text&
        get_text() const;

        render::drawable_text&
        get_text();

        void
        set_size(float size);

        void
        update_text();


        std::shared_ptr<render::drawable_font> font;
        std::string string;
        float size;
        std::shared_ptr<render::drawable_text> text;
    };
}
