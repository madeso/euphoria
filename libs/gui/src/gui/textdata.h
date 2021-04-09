#ifndef GUI_TEXTDATA_H
#define GUI_TEXTDATA_H

#include "core/noncopyable.h"

#include <memory>
#include <string>


namespace euphoria::render
{
    struct Font;
    struct Text;
}

namespace euphoria::gui
{
    struct TextData
    {
        TextData();
        ~TextData();

        NONCOPYABLE(TextData);

        void
        SetFont(std::shared_ptr<render::Font> font);

        [[nodiscard]] const render::Font&
        GetFont() const;

        void
        SetString(const std::string& str);

        [[nodiscard]] const std::string&
        GetString() const;

        [[nodiscard]] bool
        HasText() const;

        [[nodiscard]] const render::Text&
        GetText() const;

        render::Text&
        GetText();

        void
        SetSize(float size);

        void
        UpdateText();


        std::shared_ptr<render::Font> font_;
        std::string string_;
        float size;
        std::shared_ptr<render::Text> text_;
    };
}

#endif  // GUI_TEXTDATA_H

