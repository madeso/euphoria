#ifndef GUI_TEXTDATA_H
#define GUI_TEXTDATA_H

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
    struct TextData
    {
        TextData();
        ~TextData();

        NONCOPYABLE(TextData);

        void
        SetFont(std::shared_ptr<render::drawable_font> font);

        [[nodiscard]] const render::drawable_font&
        GetFont() const;

        void
        SetString(const std::string& str);

        [[nodiscard]] const std::string&
        GetString() const;

        [[nodiscard]] bool
        HasText() const;

        [[nodiscard]] const render::drawable_text&
        GetText() const;

        render::drawable_text&
        GetText();

        void
        SetSize(float size);

        void
        UpdateText();


        std::shared_ptr<render::drawable_font> font_;
        std::string string_;
        float size;
        std::shared_ptr<render::drawable_text> text_;
    };
}

#endif  // GUI_TEXTDATA_H

